// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rt1051_cellular.hpp"
#include "fsl_cache.h"

#include <EventStore.hpp>
#include <task.h>
#include <ticks.hpp>

#include <algorithm>

static bsp::cellular::CellularDMAResultStruct RXfer;

#if DEBUG_CELLULAR_UART == 1
#define logUARTdebug(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logUARTdebug(...)
#endif

extern "C"
{
    void LPUART1_IRQHandler(void)
    {
        std::uint32_t isrReg = LPUART_GetStatusFlags(CELLULAR_UART_BASE);

        if (bsp::RT1051Cellular::uartRxBuffer != NULL) {
            std::uint32_t count = 0;

            auto RxDmaStatus =
                LPUART_TransferGetReceiveCountEDMA(CELLULAR_UART_BASE, &bsp::RT1051Cellular::uartDmaHandle, &count);

            RXfer.dataSize = count;

            if ((isrReg & bsp::RT1051Cellular::edgeIRQMask) != 0u ||
                (isrReg & bsp::RT1051Cellular::dataRegFullIRQMask) != 0) {
                // logUARTdebug("[RX] edgeIRQ");
                if (RxDmaStatus == kStatus_NoTransferInProgress) {
                    // logUARTdebug("[RX] No transfer in progress");
                    LPUART_DisableInterrupts(CELLULAR_UART_BASE, bsp::RT1051Cellular::edgeIRQMaskEnable);

                    if (not bsp::RT1051Cellular::startReceive(bsp::RT1051Cellular::getMaxBufferDataSize())) {
                        bsp::RT1051Cellular::RestartReceivingManually = true;
                    }
                }
            }

            if ((isrReg & bsp::RT1051Cellular::idleIRQMask) != 0u &&
                (isrReg & bsp::RT1051Cellular::dataRegFullIRQMask) == 0u) {
                // logUARTdebug("[RX] idleIRQ");
                if (RxDmaStatus != kStatus_NoTransferInProgress) {
                    // logUARTdebug("[RX] Transfer in progress. Stopping engine");
                    LPUART_TransferAbortReceiveEDMA(CELLULAR_UART_BASE, &bsp::RT1051Cellular::uartDmaHandle);

                    if (count > 0) {
                        bsp::RT1051Cellular::sendRxDmaResult(bsp::cellular::CellularResultCode::ReceivedAndIdle);
                    }
                }
            }

            if (isrReg & kLPUART_RxOverrunFlag) {
                LOG_WARN("IRQ RX overrun");
            }
        }

        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, isrReg);
    }
};

namespace bsp
{
    std::size_t RT1051Cellular::RXdmaMaxReceivedCount = 0;
    bool RT1051Cellular::RestartReceivingManually     = false;

    using namespace drivers;

    lpuart_edma_handle_t RT1051Cellular::uartDmaHandle  = {};
    TaskHandle_t RT1051Cellular::untilReceivedNewHandle = nullptr;
    MessageBufferHandle_t RT1051Cellular::uartRxBuffer  = nullptr;

    RT1051Cellular::RT1051Cellular()
    {
        MSPInit();
        /// to set Store::GSM sim state and to log debug
        Store::GSM::get()->tray =
            GPIO_PinRead(GPIO2, BSP_CELLULAR_SIM_TRAY_INSERTED_PIN) == 0 ? Store::GSM::Tray::IN : Store::GSM::Tray::OUT;
        DMAInit();

        uartRxBuffer = xMessageBufferCreate(rxMessageBufferLength);
        if (uartRxBuffer == nullptr) {
            LOG_ERROR("Could not create the RX message buffer!");
            return;
        }

        driverLPUART = drivers::DriverLPUART::Create(
            "cellular", static_cast<drivers::LPUARTInstances>(BoardDefinitions::CELLULAR_LPUART_INSTANCE));

        lpuart_config_t s_cellularConfig;

        LPUART_GetDefaultConfig(&s_cellularConfig);

        s_cellularConfig.baudRate_Bps  = baudrate;
        s_cellularConfig.dataBitsCount = kLPUART_EightDataBits;
        s_cellularConfig.parityMode    = kLPUART_ParityDisabled;
        s_cellularConfig.isMsb         = false;
        s_cellularConfig.rxIdleType    = kLPUART_IdleTypeStartBit;
#if DEBUG_CELLULAR_UART
        s_cellularConfig.rxIdleConfig = kLPUART_IdleCharacter4; // Logs take time
#else
        s_cellularConfig.rxIdleConfig = kLPUART_IdleCharacter1;
#endif
        s_cellularConfig.enableTx    = false;
        s_cellularConfig.enableRx    = false;
        s_cellularConfig.enableTxCTS = true;
        s_cellularConfig.txCtsConfig = kLPUART_CtsSampleAtStart; // be nice, allow to stop mid txfer
        s_cellularConfig.enableRxRTS = true;

        if (LPUART_Init(CELLULAR_UART_BASE, &s_cellularConfig, GetPerphSourceClock(PerphClock_LPUART)) !=
            kStatus_Success) {
            LOG_ERROR("Could not initialize the uart!");
            return;
        }

        static_assert(rxMessageBufferLength >= RXfer.getMaxSize() + messageBufferOverheadSize,
                      "Minimum buffer size to hold one full DMA rxfer");

        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);
        NVIC_SetPriority(LPUART1_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(LPUART1_IRQn);

        enableRx();
        isInitialized = true;
    }

    void RT1051Cellular::setSpeed(uint32_t portSpeed)
    {
        LOG_DEBUG("[RT1051] Setting %" PRIu32 " baudrate", portSpeed);

        disableRx();
        LPUART_SetBaudRate(CELLULAR_UART_BASE, portSpeed, GetPerphSourceClock(PerphClock_LPUART));
        enableRx();
    }

    RT1051Cellular::~RT1051Cellular()
    {
        if (uartRxBuffer) {
            vMessageBufferDelete(uartRxBuffer);
            uartRxBuffer = nullptr;
        }

        disableRx();

        NVIC_DisableIRQ(LPUART1_IRQn);
        LPUART_DisableInterrupts(CELLULAR_UART_BASE,
                                 kLPUART_IdleLineInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);

        // Needed for the last flush in LPUART_Deinit
        enableTx();

        LPUART_Deinit(CELLULAR_UART_BASE);
        disableTx();

        MSPDeinit();
        DMADeinit();

        memset(&uartDmaHandle, 0, sizeof uartDmaHandle);
        untilReceivedNewHandle = nullptr;
    }

    void RT1051Cellular::powerUp()
    {
        constexpr TickType_t POWER_UP_DELAY_MS = 500;

        disableRx();

        exitSleep();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_UP_DELAY_MS);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);

        enableRx();
    }

    void RT1051Cellular::powerDown()
    {
        constexpr std::uint16_t POWER_DOWN_DELAY_MS = 700;

        exitSleep();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_DOWN_DELAY_MS);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);

        disableRx();
    }

    void RT1051Cellular::restart()
    {
        constexpr std::uint16_t RESET_DELAY_MS = 460;

        exitSleep();

        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 1);
        vTaskDelay(pdMS_TO_TICKS(RESET_DELAY_MS));
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 0);
    }

    ssize_t RT1051Cellular::write(void *buf, size_t nbytes)
    {
        constexpr size_t txBufferSize = 150;
        static uint8_t txBuffer[txBufferSize];

        if (nbytes > txBufferSize) {
            LOG_ERROR("TX buffer too small for given number of bytes to send!");
            assert(false);
        }

        constexpr auto maxTXCheckRetires = 5;
        if (LPUART_IsEDMATxBusy(&uartDmaHandle)) {
            for (int i = 0; i < maxTXCheckRetires; ++i) {
                LOG_INFO("Waiting for TX free %d", i);
                ulTaskNotifyTake(pdFALSE, 100);
                if (!LPUART_IsEDMATxBusy(&uartDmaHandle)) {
                    break;
                }
            }
        }

        memcpy(txBuffer, buf, nbytes);

        lpuart_transfer_t sendXfer;
        logData("TX", static_cast<uint8_t *>(buf), nbytes);
        sendXfer.data     = txBuffer;
        sendXfer.dataSize = nbytes;

        exitSleep();

        uartDmaHandle.userData = xTaskGetCurrentTaskHandle();
        SCB_CleanInvalidateDCache();

        enableTx();

        auto status = LPUART_SendEDMA(CELLULAR_UART_BASE, &uartDmaHandle, &sendXfer);

        if (status != kStatus_Success) {
            LOG_ERROR("Cellular: TX Failed! , status: %d", static_cast<int>(status));
            disableTx();
            return -1;
        }

        // wait for Tx to finish
        auto ulNotificationValue = ulTaskNotifyTake(pdFALSE, 100);

        if (ulNotificationValue == 0) {
            LOG_ERROR("Cellular Uart error: TX Transmission timeout");
            disableTx();
            return -1;
        }

        disableTx();

        return nbytes;
    }

    bool RT1051Cellular::sendRxDmaResult(bsp::cellular::CellularResultCode reason)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (RXfer.getSize() > getMaxBufferDataSize()) {
            LOG_ERROR("Cannot dump DMA buffer (%d>%d)", RXfer.getSize(), getMaxBufferDataSize());
            RestartReceivingManually = true;
            return false;
        }

        RXfer.resultCode = reason;
        logUARTdebug("[RX reason] %s", c_str(reason));

        xMessageBufferSendFromISR(uartRxBuffer, (void *)&RXfer, RXfer.getSize(), &xHigherPriorityTaskWoken);

        // logUARTdebug("Added to queue");

        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        return true;
    }

    size_t RT1051Cellular::getMaxBufferDataSize()
    {
        const auto messageOverhead = messageBufferOverheadSize + bsp::cellular::CellularDMAResultStruct::getEmptySize();

        if (const auto bytesFree = xMessageBufferSpaceAvailable(uartRxBuffer); bytesFree > messageOverhead) {
            return bytesFree - messageOverhead;
        }

        return 0;
    }

    bool RT1051Cellular::startReceive(size_t nbytes)
    {
        if (getMaxBufferDataSize() <= 0) {
            logUARTdebug("Not starting RX DMA, message buffer is full. Be aware");
            return false;
        }
        else {
            // sanitize input
            RXdmaMaxReceivedCount = std::min(nbytes, static_cast<size_t>(RXdmaBufferSize));
            // logUARTdebug("Starting DMA RX, max %d bytes", RXdmaMaxReceivedCount);
        }
        RXfer.dataSize = 0;

        // start RXfer if there is a byte incoming and no pending RXfer
        lpuart_transfer_t receiveXfer;
        receiveXfer.data     = RXfer.data;
        receiveXfer.dataSize = RXdmaMaxReceivedCount;

        // rx config
        auto status = LPUART_ReceiveEDMA(CELLULAR_UART_BASE, &uartDmaHandle, &receiveXfer);
        switch (status) {
        case kStatus_Success:
            break;
        case kStatus_LPUART_RxBusy:
            LOG_WARN("UART RX DMA already busy");
            return false;
        case kStatus_InvalidArgument:
            LOG_WARN("UART RX DMA invalid argument");
            return false;
        }
        return true;
    }

    ssize_t RT1051Cellular::read(void *buffer,
                                 size_t nbytes,
                                 std::chrono::milliseconds timeoutMs = std::chrono::milliseconds{0})
    {
        logUARTdebug("[RX] Read");
        exitSleep();

        auto timeoutTicks = pdMS_TO_TICKS(timeoutMs.count());
        if ((timeoutTicks > portMAX_DELAY) || (timeoutMs == std::chrono::milliseconds::max())) {
            timeoutTicks = portMAX_DELAY;
        }

        size_t ret = xMessageBufferReceive(uartRxBuffer, buffer, nbytes, timeoutTicks);

        if (ret > bsp::cellular::CellularDMAResultStruct::getEmptySize()) {
            logData("RX",
                    static_cast<uint8_t *>(buffer) + bsp::cellular::CellularResultStructEmptySize,
                    ret - bsp::cellular::CellularResultStructEmptySize);

            if (RestartReceivingManually) {
                logUARTdebug("[RX] resume on Paused");
                // need to start manually, as RegBuf might be already full, therefore no Active Edge Interrupt
                if (startReceive(getMaxBufferDataSize())) {
                    RestartReceivingManually = false;
                }
                else {
                    // do not lose information as whether manual start needs to be initiated and schedule next poke on
                    // Read
                    LOG_WARN("Modem is waiting to send data. Stream buffer likely too small");
                }
            }
        }

        return ret;
    }

    uint32_t RT1051Cellular::wait(std::chrono::milliseconds timeoutMs)
    {
        logUARTdebug("[WAIT]");
        if (untilReceivedNewHandle != nullptr) {
            LOG_FATAL("Wait called simultaneously from more than one thread!");
            return 0;
        }

        // no need to wait: buffer already contains something
        if (!xMessageBufferIsEmpty(uartRxBuffer)) {
            return 1;
        }

        enableRx();

        // start waiting for [timeout] until a new xfer from modem is received
        untilReceivedNewHandle = xTaskGetCurrentTaskHandle();
        auto ret               = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeoutMs.count()));
        untilReceivedNewHandle = nullptr;
        return ret;
    }

    void RT1051Cellular::informModemHostAsleep()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         !CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::informModemHostWakeup()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::enterSleep()
    {
        if (!isInSleepMode) {
            logUARTdebug("Enter sleep");
            isInSleepMode = true;

            gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 1);
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 1);

            // Host sleep information must be before UART disable
            informModemHostAsleep();
            if (driverLPUART) {
                driverLPUART->Disable();
            }
        }
    }

    void RT1051Cellular::exitSleep()
    {
        // reset sleep timer countdown
        lastCommunicationTimestamp = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());

        if (isInSleepMode) {
            logUARTdebug("Exit sleep");
            isInSleepMode = false;

            if (driverLPUART) {
                driverLPUART->Enable();
            }

            gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 0);
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 0);
            vTaskDelay(pdMS_TO_TICKS(15));

            // Host wake up information must be after UART enable
            informModemHostWakeup();
        }
    }

    void RT1051Cellular::MSPInit()
    {

        gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1), DriverGPIOParams{});
        gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2), DriverGPIOParams{});
        gpio_3 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_3), DriverGPIOParams{});

        gpio_1->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN) |
                                    1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));

        gpio_2->ClearPortInterrupts(
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));

        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));

        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));

        // INPUTS
        gpio_1->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                            .defLogic = 1,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                .defLogic = 1,
                                .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN)});

        // OUTPUTS
        gpio_1->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::USB_FUNCTION_MUX_SELECT)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIMSEL_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_PRESENCE_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN)});

        gpio_3->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN)});
        ;
        // ENABLE INTERRUPTS

        gpio_1->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));
        gpio_2->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
                                1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN));
    }

    void RT1051Cellular::MSPDeinit()
    {
        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN));
        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN));
        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN));
        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));
    }

    void RT1051Cellular::DMAInit()
    {
        dmamux =
            DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::CELLULAR_DMAMUX), DriverDMAMuxParams{});
        dma = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::CELLULAR_DMA), DriverDMAParams{});

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL));

        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL), kDmaRequestMuxLPUART1Tx);
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL), kDmaRequestMuxLPUART1Rx);

        LPUART_TransferCreateHandleEDMA(CELLULAR_UART_BASE,
                                        &uartDmaHandle,
                                        uartDMACallback,
                                        NULL,
                                        reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()),
                                        reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle()));
    }

    void RT1051Cellular::DMADeinit()
    {
        dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL));
    }

    void RT1051Cellular::uartDMACallback(LPUART_Type *base,
                                         lpuart_edma_handle_t *handle,
                                         status_t status,
                                         void *userData)
    {
        BaseType_t higherPriorTaskWoken = pdFALSE;

        switch (status) {
        case kStatus_LPUART_TxIdle: {
            logUARTdebug("[TX] Done");
            // task handle to be released in userData
            vTaskNotifyGiveFromISR((TaskHandle_t)userData, &higherPriorTaskWoken);

            portEND_SWITCHING_ISR(higherPriorTaskWoken);
            break;
        }
        case kStatus_LPUART_RxIdle:
            logUARTdebug("[RX] Chunk done. Flow control must hold the gate from now on");
            RXfer.dataSize = RXdmaMaxReceivedCount;
            if (sendRxDmaResult(cellular::CellularResultCode::ReceivedAndFull) and
                startReceive(getMaxBufferDataSize())) {
                // usual mode: append a chunk and wait for line idle to finish receive
            }
            else {
                // the auxiliary exit path on stream buffer full
                RestartReceivingManually = true;
            }

            break;
        default:
            logUARTdebug("uartDMACallback status %ld", status);
            break;
        }
    }

    void RT1051Cellular::setSendingAllowed(bool state)
    {
        pv_SendingAllowed = state;
    }
    bool RT1051Cellular::getSendingAllowed() const noexcept
    {
        return pv_SendingAllowed;
    }

    void RT1051Cellular::selectAntenna(bsp::cellular::antenna antenna)
    {
        if (antenna == bsp::cellular::antenna::lowBand) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_A_STATE);
            LOG_INFO("Selecting Antenna A");
        }
        else if (antenna == bsp::cellular::antenna::highBand) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_B_STATE);
            LOG_INFO("Selecting Antenna B");
        }
    }

    bsp::cellular::antenna RT1051Cellular::getAntenna()
    {
        // make sure ANTSEL pin has Software Input On Field set
        bool whichAntenna = gpio_2->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));
        return (whichAntenna == CELLULAR_BSP_ANTSEL_PIN_A_STATE ? bsp::cellular::antenna::lowBand
                                                                : bsp::cellular::antenna::highBand);
    }

    void RT1051Cellular::logData(const std::string &title, uint8_t *buffer, size_t nbytes)
    {
#if DEBUG_CELLULAR_UART == 1
        LOG_PRINTF("[%s: %d]", title.c_str(), nbytes);
        uint8_t *ptr = buffer;
        LOG_PRINTF("\n{");
        for (size_t i = 0; i < nbytes; i++) {
            LOG_PRINTF("%02X ", ptr[i]);
        }
        LOG_PRINTF("}\n");
#endif
    }
    bsp::Board RT1051Cellular::getBoard()
    {
        return bsp::Board::RT1051;
    }

    namespace cellular
    {
        static xQueueHandle qhandle = nullptr;

        auto init(QueueHandle_t qHandle) -> int
        {
            qhandle = qHandle;
            return 0;
        }

        namespace USB
        {
            BootPinState getBootPin()
            {
                auto gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2),
                                                 DriverGPIOParams{});
                bool state  = gpio_2->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN));

                return (state == true ? BootPinState::FIRMWARE_UPGRADE : BootPinState::NORMAL_BOOT);
            }
            PassthroughState getPassthrough()
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                bool state  = gpio_1->ReadPin(magic_enum::enum_integer(BoardDefinitions::USB_FUNCTION_MUX_SELECT));

                return (state == true ? PassthroughState::ENABLED : PassthroughState::DISABLED);
            }

            // modem needs a reboot to enter DFU (Firmware Upgrade) mode
            void setBootPin(BootPinState bootPin)
            {
                auto gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2),
                                                 DriverGPIOParams{});
                gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN),
                                 bootPin == BootPinState::FIRMWARE_UPGRADE ? 1 : 0);
            }

            void setPassthrough(PassthroughState pass)
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                gpio_1->WritePin(magic_enum::enum_integer(BoardDefinitions::USB_FUNCTION_MUX_SELECT),
                                 pass == PassthroughState::ENABLED ? 1 : 0);
            }
        } // namespace USB

        namespace status
        {
            bsp::cellular::status::value getStatus()
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                auto state =
                    gpio_1->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN)) == 0
                        ? value::ACTIVE
                        : value::INACTIVE;

                return state;
            }
            BaseType_t statusIRQhandler()
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                if (qhandle != NULL) {
                    std::uint8_t val = static_cast<std::uint8_t>(IRQsource::statusPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }

        } // namespace status

        namespace sim
        {

            auto trayIRQHandler() -> BaseType_t
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                if (qhandle) {
                    std::uint8_t val = static_cast<std::uint8_t>(IRQsource::trayPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }

            auto getTray() -> Store::GSM::Tray
            {
                auto state = GPIO_PinRead(GPIO2, BSP_CELLULAR_SIM_TRAY_INSERTED_PIN) == 0 ? Store::GSM::Tray::IN
                                                                                          : Store::GSM::Tray::OUT;
                return state;
            }

            void hotSwapTrigger()
            {
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 1);
                vTaskDelay(100); // sleep for 100 ms...
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 0);
            }

            void simSelect()
            {
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT,
                              BSP_CELLULAR_SIMSEL_PIN,
                              static_cast<uint8_t>(Store::GSM::get()->selected));
                LOG_INFO("%s slot selected", magic_enum::enum_name(Store::GSM::get()->selected).data());
            }
        } // namespace sim

        namespace ringIndicator
        {
            auto riIRQHandler() -> BaseType_t
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                if (qhandle != NULL) {
                    std::uint8_t val = static_cast<std::uint8_t>(IRQsource::ringIndicatorPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }
        } // namespace ringIndicator
    }     // namespace cellular
} // namespace bsp
