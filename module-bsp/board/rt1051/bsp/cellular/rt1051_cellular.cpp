
/*
 * @file rt1501_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "rt1051_cellular.hpp"
#include "menums/magic_enum.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "dma_config.h"
#include "fsl_cache.h"


extern "C" {

void LPUART1_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint32_t isrReg = LPUART_GetStatusFlags(CELLULAR_UART_BASE);
    static char characterReceived = 0;

    if (bsp::RT1051Cellular::uartRxStreamBuffer != NULL) {
        if (isrReg & kLPUART_RxDataRegFullFlag) {
            characterReceived = LPUART_ReadByte(CELLULAR_UART_BASE);

            if (characterReceived != 0) {
                if (xStreamBufferSpacesAvailable(bsp::RT1051Cellular::uartRxStreamBuffer) < 8) {
                    //BSP_CellularDeassertRTS();
                }
                xStreamBufferSendFromISR(bsp::RT1051Cellular::uartRxStreamBuffer,
                                         (void *) &characterReceived,
                                         1,
                                         &xHigherPriorityTaskWoken);

                if (xTimerResetFromISR(bsp::RT1051Cellular::rxTimeoutTimer, &xHigherPriorityTaskWoken) != pdTRUE) {
                    assert(0);
                }
            }

        }
    }
    LPUART_ClearStatusFlags(CELLULAR_UART_BASE, isrReg);


    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

}

namespace bsp {

    using namespace drivers;
    using namespace magic_enum;

    TimerHandle_t                   RT1051Cellular::rxTimeoutTimer = nullptr;
    lpuart_edma_handle_t            RT1051Cellular::uartDmaHandle = {};
    TaskHandle_t                    RT1051Cellular::blockedTaskHandle = nullptr;
    StreamBufferHandle_t            RT1051Cellular::uartRxStreamBuffer = nullptr;


    RT1051Cellular::RT1051Cellular() {

        MSPInit();
        DMAInit();

        uartRxStreamBuffer = xStreamBufferCreate(rxStreamBufferLength, rxStreamBufferNotifyWatermark);
        if (uartRxStreamBuffer == NULL) {
            LOG_ERROR("Could not create the RX stream buffer!");
            return;
        }

        rxTimeoutTimer = xTimerCreate
                ( /* Just a text name, not used by the RTOS
                kernel. */
                        "celltim",
                        /* The timer period in ticks, must be
                        greater than 0. */
                        25,
                        /* The timers will auto-reload themselves
                        when they expire. */
                        pdFALSE,
                        /* The ID is used to store a count of the
                        number of times the timer has expired, which
                        is initialised to 0. */
                        (void *) 0,
                        /* Each timer calls the same callback when
                        it expires. */
                        rxTimeoutTimerHandle
                );

        if (rxTimeoutTimer == nullptr) {
            LOG_ERROR("Could not create rxTimeoutTimer");
            return;
        }

        lpuart_config_t s_cellularConfig;

        LPUART_GetDefaultConfig(&s_cellularConfig);

        s_cellularConfig.baudRate_Bps = baudrate;
        s_cellularConfig.dataBitsCount = kLPUART_EightDataBits;
        s_cellularConfig.parityMode = kLPUART_ParityDisabled;
        s_cellularConfig.isMsb = false;
        s_cellularConfig.rxIdleType = kLPUART_IdleTypeStartBit;
        s_cellularConfig.rxIdleConfig = kLPUART_IdleCharacter1;
        s_cellularConfig.enableTx = false;
        s_cellularConfig.enableRx = false;

        if (LPUART_Init(CELLULAR_UART_BASE, &s_cellularConfig, UartGetPeripheralClock()) != kStatus_Success) {
            LOG_ERROR("Could not initialize the uart!");
            return;
        }

        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);
        NVIC_SetPriority(LPUART1_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(LPUART1_IRQn);

        EnableRx();

        isInitialized = true;
    }


    RT1051Cellular::~RT1051Cellular() {

        if (uartRxStreamBuffer) {
            vStreamBufferDelete(uartRxStreamBuffer);
            uartRxStreamBuffer = nullptr;
        }

        if (rxTimeoutTimer) {
            xTimerDelete(rxTimeoutTimer, 100);
            rxTimeoutTimer = nullptr;
        }

        DisableRx();
        DisableTx();

        NVIC_DisableIRQ(LPUART1_IRQn);
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);

        LPUART_Deinit(CELLULAR_UART_BASE);

        MSPDeinit();
        DMADeinit();

        memset(&uartDmaHandle, 0, sizeof uartDmaHandle);
        blockedTaskHandle = nullptr;

    }

    void RT1051Cellular::PowerUp() {
        const TickType_t POWER_UP_DELAY_MS = 500;

        DisableRx();

        WakeupModem();
        InformModemHostWakeup();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_UP_DELAY_MS);
        //BSP_CellularSetPowerState(CellularPowerStateTurningOn);
        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);

        EnableRx();
    }

    void RT1051Cellular::PowerDown() {
        const uint16_t POWER_DOWN_DELAY_MS = 700;

        WakeupModem();
        InformModemHostWakeup();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_DOWN_DELAY_MS);
        //BSP_CellularSetPowerState(CellularPowerStateTurningOff);
        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);
        //vTaskDelay(pdMS_TO_TICKS(POWER_DOWN_IN_PROGRESS_DELAY_MS));

/*        while (s_cellularPowerState != CellularPowerStatePoweredDown)
        {
            vTaskDelay(pdMS_TO_TICKS(200));
        }*/

        DisableRx();
    }

    void RT1051Cellular::Restart() {
        const uint16_t RESET_DELAY_MS = 460;

        InformModemHostWakeup();
        WakeupModem();

        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 1);
        vTaskDelay(pdMS_TO_TICKS(RESET_DELAY_MS));
        gpio_2->WritePin(enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 0);

    }

    ssize_t RT1051Cellular::Write(void *buf, size_t nbytes) {
        lpuart_transfer_t sendXfer;
        sendXfer.data = static_cast<uint8_t *>(buf);
        sendXfer.dataSize = nbytes;

        EnableTx();

        uartDmaHandle.userData = xTaskGetCurrentTaskHandle();
        SCB_CleanInvalidateDCache();
        if (LPUART_SendEDMA(CELLULAR_UART_BASE, &uartDmaHandle, &sendXfer) != kStatus_Success) {
            LOG_ERROR("Cellular: TX Failed!");
            DisableTx();
            return -1;
        }

        auto ulNotificationValue = ulTaskNotifyTake(pdFALSE, 100);

        if (ulNotificationValue == 0) {
            LOG_ERROR("Cellular Uart error: TX Transmission timeout");
            DisableTx();
            return -1;
        }

        DisableTx();
        return nbytes;
    }

    ssize_t RT1051Cellular::Read(void *buf, size_t nbytes) {
        return xStreamBufferReceive(uartRxStreamBuffer, buf, nbytes, 0);
    }

    uint32_t RT1051Cellular::Wait(uint32_t timeout) {
        if (blockedTaskHandle != nullptr) {
            LOG_FATAL("Wait called simultaneously from more than one thread!");
            return 0;
        }

        if (xStreamBufferBytesAvailable(uartRxStreamBuffer)) {
            return 1;
        }

        blockedTaskHandle = xTaskGetCurrentTaskHandle();
        auto ret = ulTaskNotifyTake(pdTRUE, timeout);
        blockedTaskHandle = nullptr;
        return ret;
    }


    void RT1051Cellular::MSPInit() {

        gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances >(BoardDefinitions::CELLULAR_GPIO_1), DriverGPIOParams{});
        gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances >(BoardDefinitions::CELLULAR_GPIO_2), DriverGPIOParams{});
        gpio_3 = DriverGPIO::Create(static_cast<GPIOInstances >(BoardDefinitions::CELLULAR_GPIO_3), DriverGPIOParams{});

        gpio_2->ClearPortInterrupts(
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_2_INSERTED_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_1_INSERTED_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN)
        );

        gpio_2->DisableInterrupt(
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_2_INSERTED_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_1_INSERTED_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
                1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN)
        );

        gpio_1->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Input,
                .irqMode=DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                .defLogic = 1,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Input,
                .irqMode=DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                .defLogic = 1,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Input,
                .irqMode=DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                .defLogic = 1,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_1_INSERTED_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Input,
                .irqMode=DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                .defLogic = 1,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_2_INSERTED_PIN)});


        gpio_1->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_1_RTS_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN)});


        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN)});


        gpio_3->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 0,
                .pin = enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN)});


/*        GPIO_PortEnableInterrupts(BSP_CELLULAR_SIM_CARD_1_INSERTED_PORT, 1U << BSP_CELLULAR_SIM_CARD_1_INSERTED_PIN);
        GPIO_PortEnableInterrupts(BSP_CELLULAR_SIM_CARD_2_INSERTED_PORT, 1U << BSP_CELLULAR_SIM_CARD_2_INSERTED_PIN);*/
    }

    void RT1051Cellular::MSPDeinit() {
        gpio_2->DisableInterrupt(1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_2_INSERTED_PIN));
        gpio_2->DisableInterrupt(1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_1_INSERTED_PIN));
        gpio_2->DisableInterrupt(1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN));
        gpio_1->DisableInterrupt(1 << enum_integer(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN));
    }

    void RT1051Cellular::DMAInit() {

        //TODO:M.P add PLL support
        //pll = DriverInterface<DriverPLL>::Create(static_cast<PLLInstances >(BoardDefinitions ::AUDIO_PLL),DriverPLLParams{});
        dmamux = DriverDMAMux::Create(static_cast<DMAMuxInstances >(BoardDefinitions::CELLULAR_DMAMUX),
                                      DriverDMAMuxParams{});
        dma = DriverDMA::Create(static_cast<DMAInstances >(BoardDefinitions::CELLULAR_DMA), DriverDMAParams{});


        txDMAHandle = dma->CreateHandle(enum_integer(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        dmamux->Enable(enum_integer(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL),
                       kDmaRequestMuxLPUART1Tx); // TODO: M.P fix BSP_CELLULAR_UART_TX_DMA_CH

        LPUART_TransferCreateHandleEDMA(CELLULAR_UART_BASE,
                                        &uartDmaHandle,
                                        DMATxCompletedCb,
                                        NULL,
                                        reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()),
                                        NULL);
    }

    void RT1051Cellular::DMADeinit() {
        dmamux->Disable(enum_integer(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
    }

    uint32_t RT1051Cellular::UartGetPeripheralClock() {
        const int UART_PERIPHERAL_PLL_DIVIDER = 6;
        uint32_t freq = 0;
        /* To make it simple, we assume default PLL and divider settings, and the only variable
           from application is use PLL3 source or OSC source */
        if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
        {
            freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) /
                   (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        } else {
            freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        }

        return freq;
    }


    void RT1051Cellular::DMATxCompletedCb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status,
                                          void *userData) {

        BaseType_t higherPriorTaskWoken = 0;
        vTaskNotifyGiveFromISR((TaskHandle_t) userData, &higherPriorTaskWoken);

        portEND_SWITCHING_ISR(higherPriorTaskWoken);
    }

    void RT1051Cellular::rxTimeoutTimerHandle(TimerHandle_t xTimer) {
        if (blockedTaskHandle) {
            xTaskNotifyGive(blockedTaskHandle);
        }

    }

}