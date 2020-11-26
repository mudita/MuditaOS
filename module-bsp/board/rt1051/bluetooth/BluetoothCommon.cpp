// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-bsp/drivers/dmamux/DriverDMAMux.hpp>
#include <module-bsp/bsp/BoardDefinitions.hpp>
#include <sstream>
#include "BluetoothWorker.hpp"
#include "bsp/bluetooth/Bluetooth.hpp"
#include "log/log.hpp"
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include "board.h"

using namespace bsp;

lpuart_edma_handle_t BluetoothCommon::uartDmaHandle = {};

// TODO it's plain copy same as in cellular - this is kind of wrong
uint32_t UartGetPeripheralClock();

void BTdev::_circ::sem_take()
{
    if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)) {
        xSemaphoreTake(sem, 0);
    }
    else {
        BaseType_t px;
        xSemaphoreTakeFromISR(sem, &px);
    }
}

void BTdev::_circ::sem_give()
{
    if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)) {
        xSemaphoreGive(sem);
    }
    else {
        BaseType_t px;
        xSemaphoreGiveFromISR(sem, &px);
    }
}

BTdev::_circ::_circ(unsigned int size, int threshold) : head(0), tail(0), threshold(threshold), size(size), len(0)
{
    buff = new char[size];
    sem  = xSemaphoreCreateBinary();
}

BTdev::_circ::~_circ()
{
    vSemaphoreDelete(sem);
    delete[] buff;
}

BluetoothCommon::BluetoothCommon(unsigned int in_size, unsigned int out_size, int threshold)
    : BTdev(in_size, out_size, threshold)
{
    configure_uart_io();
    configure_lpuart();
    configure_cts_irq();
    LOG_INFO("Bluetooth HW init done!");
}

BluetoothCommon::~BluetoothCommon()
{}

void BluetoothCommon::open()
{
    LOG_INFO("Bluetooth HW open!");
    set_irq(true);
    set_reset(true);
    // wait for the module to power up (up to 100ms according to the docs )
//    do {
//    } while (read_cts() == 1);
    is_open = true;
}

void BluetoothCommon::close()
{
    LOG_INFO("close!");
    // TODO destroy semaphore
    set_irq(false);
    is_open = false;
    set_reset(false);
}

void BluetoothCommon::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

ssize_t BluetoothCommon::read(char *buf, size_t nbytes)
{
    // start RXfer if there is byte incoming and no pending RXfer
    lpuart_transfer_t receiveXfer;
    receiveXfer.data     = reinterpret_cast<uint8_t *>(buf);
    receiveXfer.dataSize = nbytes;

    ssize_t ret = 0;

    SCB_CleanInvalidateDCache();
    auto status = LPUART_ReceiveEDMA(BSP_BLUETOOTH_UART_BASE, &BluetoothCommon::uartDmaHandle, &receiveXfer);
    switch (status) {
    case kStatus_Success:
        ret = nbytes;
        break;
    case kStatus_LPUART_RxBusy:
        ret = -1;
        break;
    case kStatus_InvalidArgument:
#ifdef DO_DEBUG_HCI_COMS
        LOG_ERROR("BT UART RX DMA invalid argument");
#endif
        ret = -1;
        break;
    }
    return ret;
}

ssize_t BluetoothCommon::write(char *buf, size_t nbytes)
{
#ifdef DO_DEBUG_HCI_COMS
    std::stringstream ss;
    for (auto i = 0; i < nbytes; ++i) {
        ss << " 0x" << std::hex << (int)buf[i];
    }
    LOG_DEBUG("write DMA -> [%d]>%s<", nbytes, ss.str().c_str());
#endif

    lpuart_transfer_t sendXfer;
    sendXfer.data     = reinterpret_cast<uint8_t *>(buf);
    sendXfer.dataSize = nbytes;

    uartDmaHandle.userData = xTaskGetCurrentTaskHandle();

    // Sends out.
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);

    ssize_t ret = 0;

    SCB_CleanInvalidateDCache();
    auto sent = LPUART_SendEDMA(BSP_BLUETOOTH_UART_BASE, &uartDmaHandle, &sendXfer);
    switch (sent) {
    case kStatus_Success: {
        // orchestrate a DMA tx
#ifdef DO_DEBUG_HCI_COMS
        LOG_DEBUG("DMA Tx pending");
#endif
        ret = nbytes;
    } break;
    case kStatus_LPUART_TxBusy:
        // could've checked beforehand
#ifdef DO_DEBUG_HCI_COMS
        LOG_ERROR("Previous DMA xfer is still pending");
#endif
        ret = -1;
        break;
    case kStatus_InvalidArgument:
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        ret = -1;
        break;
    }
    return ret;
}

ssize_t BluetoothCommon::write_blocking(char *buf, ssize_t len)
{
    ssize_t ret = -1;

    // flush RX circ buffer
    bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();
    bt->in.flush();

    auto wrote = write(buf, len);

    if (wrote == len) { // success orchestrating a transfer
        constexpr auto write_blocking_timeout = pdMS_TO_TICKS(100);
        auto ulNotificationValue              = ulTaskNotifyTake(pdFALSE, write_blocking_timeout);
        if (ulNotificationValue != 0) { // success completing a transfer
            LOG_DEBUG("DMA Tx wrote");
            ret = len;
        } else {
            LOG_ERROR("DMA Tx timeout");
            ret = -1;
        }
    }
    else{
        LOG_WARN("DMA Tx not wrote (%d/%d)", wrote, len);
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
    return ret;
}

BTdev::Error BluetoothCommon::set_reset(bool on)
{
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

void BluetoothCommon::configure_uart_io()
{
    gpio_pin_config_t gpio_init_structure;
    gpio_init_structure.direction     = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic   = 1;
    GPIO_PinInit(BSP_BLUETOOTH_UART_RTS_PORT, BSP_BLUETOOTH_UART_RTS_PIN, &gpio_init_structure);
    gpio_init_structure.direction     = kGPIO_DigitalInput;
    gpio_init_structure.interruptMode = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic   = 0;
    GPIO_PinInit(BSP_BLUETOOTH_UART_CTS_PORT, BSP_BLUETOOTH_UART_CTS_PIN, &gpio_init_structure);
    gpio_init_structure.direction     = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode = kGPIO_NoIntmode;
    gpio_init_structure.outputLogic   = 1;
    GPIO_PinInit(BSP_BLUETOOTH_OSC_EN_PORT, BSP_BLUETOOTH_OSC_EN_PIN, &gpio_init_structure);
    gpio_init_structure.direction   = kGPIO_DigitalOutput;
    gpio_init_structure.outputLogic = 0;
    GPIO_PinInit(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, &gpio_init_structure);
}

void BluetoothCommon::configure_lpuart()
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
    bt_c.enableRxRTS   = true;

    if (LPUART_Init(BSP_BLUETOOTH_UART_BASE, &bt_c, UartGetPeripheralClock()) != kStatus_Success) {
        LOG_ERROR("BT: UART config error Could not initialize the uart!");
        return;
    }

    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART2_IRQn);

    dmamux = drivers::DriverDMAMux::Create(static_cast<drivers::DMAMuxInstances>(BoardDefinitions ::BLUETOOTH_DMAMUX),
                                           drivers::DriverDMAMuxParams{});
    dma    = drivers::DriverDMA::Create(static_cast<drivers::DMAInstances>(BoardDefinitions ::BLUETOOTH_DMA),
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

void BluetoothCommon::uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
    BaseType_t taskwoken = 0;
    uint8_t val;
    bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();

    switch (status) {
    case kStatus_LPUART_TxIdle: {
#ifdef DO_DEBUG_HCI_COMS
        LOG_DEBUG("DMA irq: TX done");
#endif
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        val = Bt::Message::EvtSent;
        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        break;
    }
    case kStatus_LPUART_RxIdle:
#ifdef DO_DEBUG_HCI_COMS
        LOG_DEBUG("DMA irq: RX done");
#endif
        val = Bt::Message::EvtReceived;
        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        portEND_SWITCHING_ISR(taskwoken);
        break;
    default:
#ifdef DO_DEBUG_HCI_COMS
        LOG_DEBUG("DMA irq: something else! (%ld). Quite impossible", status);
#endif
        break;
    }
}

void BluetoothCommon::configure_cts_irq()
{
    DisableIRQ(GPIO1_Combined_16_31_IRQn);
    GPIO_PortClearInterruptFlags(GPIO1, 0xFFFFFFFF);
    GPIO_PortEnableInterrupts(GPIO1, (1 << BSP_BLUETOOTH_UART_CTS_PIN));
    EnableIRQ(GPIO1_Combined_16_31_IRQn);
    NVIC_SetPriority(GPIO1_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
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
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, true);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);
}

extern "C"
{
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg      = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        BaseType_t taskwoken = 0;
        uint8_t val          = Bt::Message::EvtReceived;
        bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();

        if (isrReg & kLPUART_RxDataRegFullFlag) {
            LOG_WARN("LPUART IRQ RX full");
        }
        if (isrReg & kLPUART_RxOverrunFlag) {
            LOG_WARN("LPUART IRQ RX overrun");
            val = Bt::Message::EvtUartError;
            xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
}
