// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <log/log.hpp>
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

namespace
{
    std::uint32_t uartGetPeripheralClock()
    {
        constexpr auto UART_PERIPHERAL_PLL_DIVIDER = 6;
        std::uint32_t freq;

        /* To make it simple, we assume default PLL and divider settings, and the
         * only variable from application is use PLL3 source or OSC source */
        if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
        {
            freq =
                (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        }
        else {
            freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        }

        return freq;
    }
} // namespace

lpuart_edma_handle_t BluetoothCommon::uartDmaHandle = {};

BluetoothCommon::BluetoothCommon()
{
    gpio_pin_config_t gpioConfig;
    gpioConfig.direction     = kGPIO_DigitalOutput;
    gpioConfig.interruptMode = kGPIO_NoIntmode;
    gpioConfig.outputLogic   = 0;
    GPIO_PinInit(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, &gpioConfig);
}

BluetoothCommon::~BluetoothCommon() = default;

void BluetoothCommon::open()
{
    uartInit();
    uartDmaInit();
    LOG_INFO("Bluetooth HW init done from open!");

    setIrq(true);
    setReset(true);
    isOpen = true;
    LOG_INFO("Bluetooth HW open!");
}

void BluetoothCommon::close()
{
    setReset(false);
    setIrq(false);
    isOpen = false;

    uartDmaDeinit();
    uartDeinit();
    LOG_INFO("Bluetooth HW close!");
}

void BluetoothCommon::sleepMs(std::size_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTDevice::Error BluetoothCommon::read(std::uint8_t *buf, std::size_t size)
{
    auto ret = ErrorUndefined;

    // Start RXfer if there is a byte incoming and no pending RXfer
    lpuart_transfer_t receiveXfer;
    receiveXfer.data     = buf;
    receiveXfer.dataSize = size;

    // Rx config
    SCB_CleanInvalidateDCache();
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, true);

    const auto status = LPUART_ReceiveEDMA(BSP_BLUETOOTH_UART_BASE, &uartDmaHandle, &receiveXfer);
    switch (status) {
    case kStatus_Success:
        ret = Success;
        break;
    case kStatus_LPUART_RxBusy:
        ret = ErrorBSP;
        LOG_WARN("BT UART Rx DMA already busy");
        break;
    case kStatus_InvalidArgument:
        LOG_WARN("BT UART Rx DMA invalid argument");
        ret = ErrorBSP;
        break;
    default:
        LOG_WARN("Unhandled case, status %ld", status);
        break;
    }
    return ret;
}

BTDevice::Error BluetoothCommon::write(const std::uint8_t *buf, std::size_t size)
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
    sendXfer.data     = const_cast<std::uint8_t *>(buf);
    sendXfer.dataSize = size;

    uartDmaHandle.userData = xTaskGetCurrentTaskHandle();

    // Tx config
    SCB_CleanInvalidateDCache();
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);

    const auto status = LPUART_SendEDMA(BSP_BLUETOOTH_UART_BASE, &uartDmaHandle, &sendXfer);
    switch (status) {
    case kStatus_Success:
        // Orchestrate a DMA Tx
        logHciComs("DMA Tx started (%d)", size);
        ret = Success;
        break;
    case kStatus_LPUART_TxBusy:
        // Could've checked beforehand
        LOG_WARN("Previous DMA Tx is still pending");
        ret = ErrorBSP;
        break;
    case kStatus_InvalidArgument:
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        LOG_ERROR("DMA Tx invalid arg");
        ret = ErrorBSP;
        break;
    default:
        LOG_WARN("Unhandled case, status %ld", status);
        break;
    }
    return ret;
}

BTDevice::Error BluetoothCommon::setBaudrate(std::uint32_t baud)
{
    LOG_INFO("Set baudrate: %" PRIu32, baud);
    Error ret = Success;
    const auto err = LPUART_SetBaudRate(BSP_BLUETOOTH_UART_BASE, baud, uartGetPeripheralClock());
    if (err) {
        LOG_ERROR("BT: baudrate %lu, error %ld", baud, err);
        ret = ErrorBSP;
    }
    BSP_BLUETOOTH_UART_BASE->FIFO |= LPUART_FIFO_RXFLUSH_MASK; // Flush fifo
    BSP_BLUETOOTH_UART_BASE->FIFO &= ~LPUART_FIFO_RXFE_MASK;   // Disable fifo
    return ret;
}

BTDevice::Error BluetoothCommon::setReset(bool on)
{
    if (on) {
        GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, 0);
        sleepMs(5 + 2); // docs: "nSHUTD must be low for a minimum of 5 ms."
    }
    LOG_INFO("reset: %s", on ? "on" : "off");
    GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, on ? 1U : 0U);
    return Success;
}

void BluetoothCommon::uartInit()
{
    lpuart_config_t uartConfig;
    LPUART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps  = defaultBaudRate;
    uartConfig.dataBitsCount = kLPUART_EightDataBits;
    uartConfig.parityMode    = kLPUART_ParityDisabled;
    uartConfig.isMsb         = false;
    uartConfig.rxIdleType    = kLPUART_IdleTypeStartBit;
    uartConfig.rxIdleConfig  = kLPUART_IdleCharacter1;
    uartConfig.enableTx      = false;
    uartConfig.enableRx      = false;
    uartConfig.enableTxCTS   = true;
    uartConfig.txCtsConfig   = kLPUART_CtsSampleAtStart; // To be able to stop TX mid-transfer
    uartConfig.enableRxRTS   = true; // == BSP_BLUETOOTH_UART_BASE->MODIR |= LPUART_MODIR_RXRTSE_MASK;

    if (LPUART_Init(BSP_BLUETOOTH_UART_BASE, &uartConfig, uartGetPeripheralClock()) != kStatus_Success) {
        LOG_ERROR("BT: UART config error, could not initialize!");
        return;
    }
    BSP_BLUETOOTH_UART_BASE->MODIR |= LPUART_MODIR_TXRTSPOL_MASK; // Apparently docs are not clear here. HIGH during Tx

    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART2_IRQn);
}

void BluetoothCommon::uartDmaInit()
{
    dmamux = drivers::DriverDMAMux::Create(static_cast<drivers::DMAMuxInstances>(BoardDefinitions::BLUETOOTH_DMAMUX),
                                           drivers::DriverDMAMuxParams{});
    dma    = drivers::DriverDMA::Create(static_cast<drivers::DMAInstances>(BoardDefinitions::BLUETOOTH_DMA),
                                     drivers::DriverDMAParams{});

    uartTxDmaHandle = dma->CreateHandle(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL));
    uartRxDmaHandle = dma->CreateHandle(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL));

    dmamux->Enable(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL), kDmaRequestMuxLPUART2Tx);
    dmamux->Enable(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL), kDmaRequestMuxLPUART2Rx);

    LPUART_TransferCreateHandleEDMA(BSP_BLUETOOTH_UART_BASE,
                                    &uartDmaHandle,
                                    uartDmaCallback,
                                    nullptr,
                                    reinterpret_cast<edma_handle_t *>(uartTxDmaHandle->GetHandle()),
                                    reinterpret_cast<edma_handle_t *>(uartRxDmaHandle->GetHandle()));
}

void BluetoothCommon::uartDeinit()
{
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);

    NVIC_DisableIRQ(LPUART2_IRQn);
    LPUART_DisableInterrupts(CELLULAR_UART_BASE, kLPUART_RxOverrunInterruptEnable);
    LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);

    LPUART_Deinit(BSP_BLUETOOTH_UART_BASE);
}

void BluetoothCommon::uartDmaDeinit()
{
    dmamux->Disable(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_RX_DMA_CHANNEL));
    dmamux->Disable(static_cast<std::uint32_t>(BoardDefinitions::BLUETOOTH_TX_DMA_CHANNEL));
}

void BluetoothCommon::uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
    std::uint8_t val;
    BaseType_t xHigherPriorityTaskWoken = 0;
    auto &blueKitchen                   = bsp::BlueKitchen::getInstance();

    switch (status) {
    case kStatus_LPUART_TxIdle: {
        logHciComs("DMA IRQ: TX done");
        LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
        val = bluetooth::Message::EvtSent;
        xQueueSendFromISR(blueKitchen.qHandle, &val, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        break;
    }
    case kStatus_LPUART_RxIdle:
        logHciComs("DMA IRQ: RX done");
        LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
        val = bluetooth::Message::EvtReceived;
        xQueueSendFromISR(blueKitchen.qHandle, &val, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        break;
    default:
        logHciComs("DMA IRQ: unhandled case");
        break;
    }
}

void BluetoothCommon::setIrq(bool enable)
{
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
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint8_t val          = bluetooth::Message::EvtReceived;
        uint32_t isrReg                     = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        bsp::BlueKitchen &blueKitchen       = bsp::BlueKitchen::getInstance();

        if (isrReg & kLPUART_RxDataRegFullFlag) {
            LOG_WARN("Bluetooth IRQ Rx full");
        }
        if (isrReg & kLPUART_RxOverrunFlag) {
            LOG_WARN("Bluetooth IRQ Rx overrun");
            val = bluetooth::Message::EvtUartError;
            xQueueSendFromISR(blueKitchen.qHandle, &val, &xHigherPriorityTaskWoken);
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
}
