// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <log.hpp>
#include "FreeRTOS.h"
#include "board.h"
#include "fsl_lpuart_edma.h"
#include <board/BoardDefinitions.hpp>

#if DEBUG_BLUETOOTH_HCI_COMS == 1
#define logHciComs(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciComs(...)
#endif

#if DEBUG_BLUETOOTH_HCI_BYTES == 1
#include <sstream>
#define logHciBytes(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciBytes(...)
#endif

using namespace bsp;

lpuart_edma_handle_t BluetoothCommon::uartDmaHandle = {};

uint32_t UartGetPeripheralClock();

BluetoothCommon::BluetoothCommon()
{
    gpio_pin_config_t gpio_init_structure;
    gpio_init_structure.direction     = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode = kGPIO_NoIntmode;
    gpio_init_structure.outputLogic   = 0;
    GPIO_PinInit(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, &gpio_init_structure);
}

BluetoothCommon::~BluetoothCommon() = default;

void BluetoothCommon::open()
{
    init_uart();
    init_uart_dma();
    LOG_INFO("Bluetooth HW init done from open!");
    set_irq(true);
    set_reset(true);
    is_open = true;
    LOG_INFO("Bluetooth HW open!");
}

void BluetoothCommon::close()
{
    set_irq(false);
    is_open = false;
    set_reset(false);
    deinit_uart_dma();
    deinit_uart();
    LOG_INFO("Bluetooth HW close!");
}

void BluetoothCommon::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTdev::Error BluetoothCommon::read(uint8_t *buf, size_t nbytes)
{
    auto ret = ErrorUndefined;

    // start RXfer if there is a byte incoming and no pending RXfer
    lpuart_transfer_t receiveXfer;
    receiveXfer.data     = buf;
    receiveXfer.dataSize = nbytes;

    // rx config
    SCB_CleanInvalidateDCache();
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, true);

    auto status = LPUART_ReceiveEDMA(BSP_BLUETOOTH_UART_BASE, &uartDmaHandle, &receiveXfer);
    switch (status) {
    case kStatus_Success:
        ret = Success;
        break;
    case kStatus_LPUART_RxBusy:
        ret = ErrorBSP;
        LOG_WARN("BT UART RX DMA already busy");
        break;
    case kStatus_InvalidArgument:
        LOG_WARN("BT UART RX DMA invalid argument");
        ret = ErrorBSP;
        break;
    }
    return ret;
}

BTdev::Error BluetoothCommon::write(const uint8_t *buf, size_t size)
{
    logHciBytes("BT DMA to write --> [%d]>%s<",
                size,
                [&]() -> std::string {
                    std::stringstream ss;
                    for (int i = 0; i < size; ++i) {
                        ss << " 0x" << std::hex << (int)buf[i];
                    }
                    return ss.str();
                }()
                             .c_str());
    auto ret = ErrorUndefined;

    lpuart_transfer_t sendXfer;
    sendXfer.data     = const_cast<uint8_t *>(buf);
    sendXfer.dataSize = size;

    uartDmaHandle.userData = xTaskGetCurrentTaskHandle();

    // tx config
    SCB_CleanInvalidateDCache();
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);

    auto sent = LPUART_SendEDMA(BSP_BLUETOOTH_UART_BASE, &uartDmaHandle, &sendXfer);
    switch (sent) {
    case kStatus_Success:
        // orchestrate a DMA Tx
        logHciComs("DMA Tx started (%d)", size);
        ret = Success;
        break;
    case kStatus_LPUART_TxBusy:
        // could've checked beforehand
        LOG_WARN("Previous DMA Tx is still pending");
        ret = ErrorBSP;
        break;
    case kStatus_InvalidArgument:
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        LOG_ERROR("DMA Tx invalid arg");
        ret = ErrorBSP;
        break;
    }
    return ret;
}

ssize_t BluetoothCommon::write_blocking(const uint8_t *buf, ssize_t nbytes)
{
    ssize_t ret = -1;

    auto wrote = write(const_cast<uint8_t *>(buf), nbytes);

    if (wrote == nbytes) { // success orchestrating a transfer
        constexpr auto writeBlockingTimeout = pdMS_TO_TICKS(100);
        auto ulNotificationValue            = ulTaskNotifyTake(pdFALSE, writeBlockingTimeout);
        if (ulNotificationValue != 0) { // success completing a transfer
            logHciComs("DMA Tx wrote");
            ret = nbytes;
        }
        else {
            LOG_ERROR("DMA Tx timeout");
            ret = -1;
        }
    }
    else {
        LOG_WARN("DMA Tx not wrote (%d/%d)", wrote, nbytes);
    }
    return ret;
}

BTdev::Error BluetoothCommon::set_baudrate(uint32_t bd)
{
    LOG_INFO("Set baudrate: %" PRIu32, bd);
    Error ret = Success;
    int err   = 0;
    if ((err = LPUART_SetBaudRate(BSP_BLUETOOTH_UART_BASE, bd, UartGetPeripheralClock())) != 0) {
        LOG_ERROR("BT error: baudrate [%lu] set err: %d", bd, err);
        ret = ErrorBSP;
    }
    BSP_BLUETOOTH_UART_BASE->FIFO |= LPUART_FIFO_RXFLUSH_MASK; // flush fifo
    BSP_BLUETOOTH_UART_BASE->FIFO &= ~LPUART_FIFO_RXFE_MASK;   // disable fifo
    return ret;
}

BTdev::Error BluetoothCommon::set_reset(bool on)
{
    if (on) {
        GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, 0);
        // docs: "nSHUTD must be low for a minimum of 5 ms."
        sleep_ms(5 + 2);
    }
    LOG_INFO("reset %s", on ? "on" : "off");
    GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, on ? 1U : 0U);
    return Success;
}

uint32_t UartGetPeripheralClock()
{
    const int UART_PERIPHERAL_PLL_DIVIDER = 6;
    uint32_t freq                         = 0;
    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}

void BluetoothCommon::init_uart()
{
    lpuart_config_t bt_c;
    LPUART_GetDefaultConfig(&bt_c);
    bt_c.baudRate_Bps  = baudrate;
    bt_c.dataBitsCount = kLPUART_EightDataBits;
    bt_c.parityMode    = kLPUART_ParityDisabled;
    bt_c.isMsb         = false;
    bt_c.rxIdleType    = kLPUART_IdleTypeStartBit;
    bt_c.rxIdleConfig  = kLPUART_IdleCharacter1;
    bt_c.enableTx      = false;
    bt_c.enableRx      = false;
    bt_c.enableTxCTS   = true;
    bt_c.txCtsConfig   = kLPUART_CtsSampleAtStart; // to be able to stop TX mid-transfer
    bt_c.enableRxRTS   = true;                     // == BSP_BLUETOOTH_UART_BASE->MODIR |= LPUART_MODIR_RXRTSE_MASK;

    if (LPUART_Init(BSP_BLUETOOTH_UART_BASE, &bt_c, UartGetPeripheralClock()) != kStatus_Success) {
        LOG_ERROR("BT: UART config error Could not initialize the uart!");
        return;
    }
    BSP_BLUETOOTH_UART_BASE->MODIR |= LPUART_MODIR_TXRTSPOL_MASK; // apparently docs are not clear here. HIGH during TX

    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART2_IRQn);
}

void BluetoothCommon::init_uart_dma()
{
    dmamux = drivers::DriverDMAMux::Create(static_cast<drivers::DMAMuxInstances>(BoardDefinitions::BLUETOOTH_DMAMUX),
                                           drivers::DriverDMAMuxParams{});
    dma    = drivers::DriverDMA::Create(static_cast<drivers::DMAInstances>(BoardDefinitions::BLUETOOTH_DMA),
                                     drivers::DriverDMAParams{});

    uartTxDmaHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL));
    uartRxDmaHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL));

    dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL), kDmaRequestMuxLPUART2Tx);
    dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL), kDmaRequestMuxLPUART2Rx);

    LPUART_TransferCreateHandleEDMA(BSP_BLUETOOTH_UART_BASE,
                                    &uartDmaHandle,
                                    uartDmaCallback,
                                    nullptr,
                                    reinterpret_cast<edma_handle_t *>(uartTxDmaHandle->GetHandle()),
                                    reinterpret_cast<edma_handle_t *>(uartRxDmaHandle->GetHandle()));
}

void BluetoothCommon::deinit_uart()
{
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);

    NVIC_DisableIRQ(LPUART2_IRQn);
    LPUART_DisableInterrupts(CELLULAR_UART_BASE, kLPUART_RxOverrunInterruptEnable);
    LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);

    LPUART_Deinit(BSP_BLUETOOTH_UART_BASE);
}

void BluetoothCommon::deinit_uart_dma()
{
    dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL));
    dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL));
}

void BluetoothCommon::uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
    BaseType_t taskwoken = 0;
    uint8_t val;
    bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();

    switch (status) {
    case kStatus_LPUART_TxIdle: {
        logHciComs("DMA irq: TX done");
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        val = bluetooth::Message::EvtSent;
        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        portEND_SWITCHING_ISR(taskwoken);
        break;
    }
    case kStatus_LPUART_RxIdle:
        logHciComs("DMA irq: RX done");
        LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
        val = bluetooth::Message::EvtReceived;
        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        portEND_SWITCHING_ISR(taskwoken);
        break;
    }
}

void BluetoothCommon::set_irq(bool enable)
{
    // printf("%s\n", __FUNCTION__);
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    if (enable) {
        LPUART_EnableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxOverrunInterruptEnable);
    }
    else {
        LPUART_DisableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxOverrunInterruptEnable);
    }
}

extern "C"
{
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg      = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        BaseType_t taskwoken = 0;
        uint8_t val          = bluetooth::Message::EvtReceived;
        bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();

        if (isrReg & kLPUART_RxDataRegFullFlag) {
            LOG_WARN("Bluetooth IRQ RX full");
        }
        if (isrReg & kLPUART_RxOverrunFlag) {
            LOG_WARN("Bluetooth IRQ RX overrun");
            val = bluetooth::Message::EvtUartError;
            xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
}
