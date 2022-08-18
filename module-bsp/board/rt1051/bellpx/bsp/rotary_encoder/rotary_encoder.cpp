// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rotary_encoder.hpp"
#include <bsp/KeyInputCommon.hpp>
#include <board/BoardDefinitions.hpp>
#include <timers.h>
#include <fsl_common.h>
#include <fsl_qtmr.h>
#include <log/log.hpp>

/* Hardware PINS
 * TMR3_CH0-----GPIO_B0_06-----
 * TMR3_CH1-----GPIO_B0_07-----
 */
namespace bsp::rotary_encoder
{
    namespace
    {
        enum class Direction
        {
            forward   = 0x01,
            backward  = 0x02,
            undefined = 0xFF
        };
        constexpr auto MAX_PER_CHECK          = 100;
        constexpr auto PRIMARY_SOURCE         = kQTMR_ClockCounter0InputPin;
        constexpr auto BOARD_QTMR_ENC_CHANNEL = kQTMR_Channel_0;
        constexpr auto SECONDARY_SOURCE       = kQTMR_Counter1InputPin;
        constexpr auto INTERRUPT_MODE         = kQTMR_EdgeInterruptEnable;
        auto BOARD_QTMR_ID                    = TMR3;
        xQueueHandle gHandleIrq;
        uint32_t encCounter;
    } // namespace

    void init(xQueueHandle qHandle)
    {
        gHandleIrq = qHandle;
        qtmr_config_t timerCfg;
        QTMR_GetDefaultConfig(&timerCfg);
        timerCfg.primarySource   = PRIMARY_SOURCE;
        timerCfg.secondarySource = SECONDARY_SOURCE;
        QTMR_Init(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, &timerCfg);
        QTMR_SetupInputCapture(
            BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, SECONDARY_SOURCE, false, false, kQTMR_RisingAndFallingEdge);
        QTMR_EnableInterrupts(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, INTERRUPT_MODE);
        QTMR_StartTimer(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, kQTMR_QuadCountMode);

        LOG_DEBUG("Init rotary encoder driver");
    }

    void deinit()
    {
        gHandleIrq = nullptr;
        QTMR_DisableInterrupts(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, INTERRUPT_MODE);
        QTMR_Deinit(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL);
    }

    std::vector<KeyEvent> getKeyEvents()
    {
        uint16_t tmp   = QTMR_GetCurrentTimerCount(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL);
        auto direction = Direction::undefined;
        std::vector<KeyEvent> out;

        int difference = tmp - encCounter;
        if (difference > MAX_PER_CHECK) {
            direction = Direction::backward;
        }
        else if (difference < -MAX_PER_CHECK) {
            direction = Direction::forward;
        }
        else if (difference > 0) {
            direction = Direction::forward;
        }
        else if (difference < 0) {
            direction = Direction::backward;
        }
        encCounter = tmp;

        if (direction == Direction::forward) {
            out.push_back({KeyCodes::JoystickUp, KeyEvents::Moved});
        }
        else if (direction == Direction::backward) {
            out.push_back({KeyCodes::JoystickDown, KeyEvents::Moved});
        }

        return out;
    }

    BaseType_t IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (gHandleIrq != nullptr) {
            std::uint8_t val = static_cast<std::uint8_t>(NotificationSource::rotaryEncoder);
            xQueueSendFromISR(gHandleIrq, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }

} // namespace bsp::rotary_encoder
