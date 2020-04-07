
/*
 * @file rt1501_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "rt1051_cellular.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "../pit/pit.hpp"
#include "dma_config.h"
#include "fsl_cache.h"
#include <common_data/EventStore.hpp>
#include <map>

namespace bsp::cellular
{
    void readline_timeout()
    {
        BaseType_t hp = pdFALSE;
        if (bsp::RT1051Cellular::blockedTaskHandle) {
            vTaskNotifyGiveFromISR(bsp::RT1051Cellular::blockedTaskHandle, &hp);
        }
        portEND_SWITCHING_ISR(hp);
    }
}; // namespace bsp::cellular

extern "C"
{
    void LPUART1_IRQHandler(void)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        uint32_t isrReg               = LPUART_GetStatusFlags(CELLULAR_UART_BASE);
        static char characterReceived = 0;

        if (bsp::RT1051Cellular::uartRxStreamBuffer != NULL) {
            if (isrReg & kLPUART_RxDataRegFullFlag) {
                characterReceived = LPUART_ReadByte(CELLULAR_UART_BASE);

                if (characterReceived != 0) {
                    if (xStreamBufferSpacesAvailable(bsp::RT1051Cellular::uartRxStreamBuffer) < 8) {
                        LOG_FATAL("...");
                    }
                    xStreamBufferSendFromISR(bsp::RT1051Cellular::uartRxStreamBuffer,
                                             (void *)&characterReceived,
                                             1,
                                             &xHigherPriorityTaskWoken);
                    bsp::pit::start(25 * 1000, bsp::cellular::readline_timeout);
                }
            }
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, isrReg);

            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
    }
};

namespace bsp
{

    using namespace drivers;

    lpuart_edma_handle_t RT1051Cellular::uartDmaHandle      = {};
    TaskHandle_t RT1051Cellular::blockedTaskHandle          = nullptr;
    StreamBufferHandle_t RT1051Cellular::uartRxStreamBuffer = nullptr;

    RT1051Cellular::RT1051Cellular()
    {
        bsp::pit::init(nullptr);

        MSPInit();
        /// to set Store::GSM sim state and to log debug
        Store::GSM::get()->tray =
            GPIO_PinRead(GPIO2, BSP_CELLULAR_SIM_TRAY_INSERTED_PIN) == 0 ? Store::GSM::Tray::IN : Store::GSM::Tray::OUT;
        DMAInit();

        uartRxStreamBuffer = xStreamBufferCreate(rxStreamBufferLength, rxStreamBufferNotifyWatermark);
        if (uartRxStreamBuffer == NULL) {
            LOG_ERROR("Could not create the RX stream buffer!");
            return;
        }

        lpuart_config_t s_cellularConfig;

        LPUART_GetDefaultConfig(&s_cellularConfig);

        s_cellularConfig.baudRate_Bps  = baudrate;
        s_cellularConfig.dataBitsCount = kLPUART_EightDataBits;
        s_cellularConfig.parityMode    = kLPUART_ParityDisabled;
        s_cellularConfig.isMsb         = false;
        s_cellularConfig.rxIdleType    = kLPUART_IdleTypeStartBit;
        s_cellularConfig.rxIdleConfig  = kLPUART_IdleCharacter1;
        s_cellularConfig.enableTx      = false;
        s_cellularConfig.enableRx      = false;

        if (LPUART_Init(CELLULAR_UART_BASE, &s_cellularConfig, GetPerphSourceClock(PerphClock_LPUART)) !=
            kStatus_Success) {
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

    void RT1051Cellular::SetSpeed(uint32_t portSpeed)
    {
        LOG_DEBUG("[RT1051] Setting %" PRIu32 " baudrate", portSpeed);

        LPUART_SetBaudRate(CELLULAR_UART_BASE, portSpeed, GetPerphSourceClock(PerphClock_LPUART));
    }

    RT1051Cellular::~RT1051Cellular()
    {

        if (uartRxStreamBuffer) {
            vStreamBufferDelete(uartRxStreamBuffer);
            uartRxStreamBuffer = nullptr;
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

    void RT1051Cellular::PowerUp()
    {
        const TickType_t POWER_UP_DELAY_MS = 500;

        DisableRx();

        ExitSleep();
        InformModemHostWakeup();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_UP_DELAY_MS);
        // BSP_CellularSetPowerState(CellularPowerStateTurningOn);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);

        EnableRx();
    }

    void RT1051Cellular::PowerDown()
    {
        const uint16_t POWER_DOWN_DELAY_MS = 700;

        ExitSleep();
        InformModemHostWakeup();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_DOWN_DELAY_MS);
        // BSP_CellularSetPowerState(CellularPowerStateTurningOff);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);
        // vTaskDelay(pdMS_TO_TICKS(POWER_DOWN_IN_PROGRESS_DELAY_MS));

        /*        while (s_cellularPowerState != CellularPowerStatePoweredDown)
                {
                    vTaskDelay(pdMS_TO_TICKS(200));
                }*/

        DisableRx();
    }

    void RT1051Cellular::Restart()
    {
        const uint16_t RESET_DELAY_MS = 460;

        InformModemHostWakeup();
        ExitSleep();

        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 1);
        vTaskDelay(pdMS_TO_TICKS(RESET_DELAY_MS));
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 0);
    }

    ssize_t RT1051Cellular::Write(void *buf, size_t nbytes)
    {
        lpuart_transfer_t sendXfer;
#if _RT1051_UART_DEBUG
        LOG_PRINTF("[TX] ");
        uint8_t *ptr = (uint8_t *)buf;
        for (size_t i = 0; i < nbytes; i++)
            LOG_PRINTF("%02X ", (uint8_t)*ptr++);
        LOG_PRINTF("\n");
#endif
        sendXfer.data     = static_cast<uint8_t *>(buf);
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

    ssize_t RT1051Cellular::Read(void *buf, size_t nbytes)
    {
        ssize_t ret = xStreamBufferReceive(uartRxStreamBuffer, buf, nbytes, 0);
#if _RT1051_UART_DEBUG
        if (ret > 0) {
            LOG_PRINTF("[RX] ");
            uint8_t *ptr = (uint8_t *)buf;
            for (size_t i = 0; i < ret; i++)
                LOG_PRINTF("%02X ", (uint8_t)*ptr++);
            LOG_PRINTF("\n");
        }
#endif
        return ret;
    }

    uint32_t RT1051Cellular::Wait(uint32_t timeout)
    {
        if (blockedTaskHandle != nullptr) {
            LOG_FATAL("Wait called simultaneously from more than one thread!");
            return 0;
        }

        if (xStreamBufferBytesAvailable(uartRxStreamBuffer)) {
            return 1;
        }

        blockedTaskHandle = xTaskGetCurrentTaskHandle();
        auto ret          = ulTaskNotifyTake(pdTRUE, timeout);
        blockedTaskHandle = nullptr;
        return ret;
    }

    void RT1051Cellular::InformModemHostAsleep()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         !CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::InformModemHostWakeup()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::EnterSleep()
    {
        gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 1);
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 1);
    }

    void RT1051Cellular::ExitSleep()
    {
        gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 0);
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 0);
        vTaskDelay(pdMS_TO_TICKS(15));
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

        gpio_1->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                            .defLogic = 1,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN)});

        gpio_1->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
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
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_RTS_PIN)});

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

        gpio_3->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN)});
        // ENABLE INTERRUPTS

        gpio_1->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));
        gpio_2->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN));
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

        // TODO:M.P add PLL support
        // pll = DriverInterface<DriverPLL>::Create(static_cast<PLLInstances >(BoardDefinitions
        // ::AUDIO_PLL),DriverPLLParams{});
        dmamux =
            DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::CELLULAR_DMAMUX), DriverDMAMuxParams{});
        dma = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::CELLULAR_DMA), DriverDMAParams{});

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL),
                       kDmaRequestMuxLPUART1Tx); // TODO: M.P fix BSP_CELLULAR_UART_TX_DMA_CH

        LPUART_TransferCreateHandleEDMA(CELLULAR_UART_BASE,
                                        &uartDmaHandle,
                                        DMATxCompletedCb,
                                        NULL,
                                        reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()),
                                        NULL);
    }

    void RT1051Cellular::DMADeinit()
    {
        dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
    }

    void RT1051Cellular::DMATxCompletedCb(LPUART_Type *base,
                                          lpuart_edma_handle_t *handle,
                                          status_t status,
                                          void *userData)
    {

        BaseType_t higherPriorTaskWoken = 0;
        vTaskNotifyGiveFromISR((TaskHandle_t)userData, &higherPriorTaskWoken);

        portEND_SWITCHING_ISR(higherPriorTaskWoken);
    }

    void RT1051Cellular::SetSendingAllowed(bool state)
    {
        pv_SendingAllowed = state;
    }
    bool RT1051Cellular::GetSendingAllowed()
    {
        return pv_SendingAllowed;
    }

    void RT1051Cellular::SelectAntenna(uint8_t antenna)
    {
        if (antenna == 0) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_A_STATE);
            LOG_INFO("Selecting Antenna A");
        }
        else if (antenna == 1) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_B_STATE);
            LOG_INFO("Selecting Antenna B");
        }
    }

    uint8_t RT1051Cellular::GetAntenna()
    {
        // make sure ANTSEL pin has Software Input On Field set
        bool whichAntenna = gpio_2->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));
        return (whichAntenna == CELLULAR_BSP_ANTSEL_PIN_A_STATE ? 0 : 1);
    }

    namespace cellular
    {
        static xQueueHandle qhandle = nullptr;

        auto init(QueueHandle_t qHandle) -> int
        {
            qhandle = qHandle;
            return 0;
        }

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
                    uint8_t val = static_cast<uint8_t>(IRQsource::statusPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }

        } // namespace status

        namespace sim
        {

            auto trayIRQ_handler() -> BaseType_t
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                if (qhandle) {
                    uint8_t val = static_cast<uint8_t>(IRQsource::trayPin);
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

            void hotswap_trigger()
            {
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 1);
                vTaskDelay(100); // sleep for 100 ms...
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 0);
            }

            void sim_sel()
            {
                if (Store::GSM::get()->selected == Store::GSM::SIM::SIM2) {
                    GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIMSEL_PIN, 1);
                }
                else {
                    GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIMSEL_PIN, 0);
                }
            }
        } // namespace sim
    }     // namespace cellular
} // namespace bsp
