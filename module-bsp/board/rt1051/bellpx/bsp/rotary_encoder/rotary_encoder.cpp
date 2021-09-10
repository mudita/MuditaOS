// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/rotary_encoder/rotary_encoder.hpp>
#include <board/BoardDefinitions.hpp>
#include <timers.h>
#include <fsl_common.h>
#include <fsl_qtmr.h>

/* Hardware PINS
 * TMR3_CH0-----GPIO_B0_06-----
 * TMR3_CH1-----GPIO_B0_07-----
 */
namespace bsp::rotary_encoder
{
    namespace
    {
        constexpr auto POLL_INTERVAL_MS       = 100U;
        constexpr uint16_t MAX_PER_100MS      = 100U;
        constexpr auto PRIMARY_SOURCE         = kQTMR_ClockCounter0InputPin;
        constexpr auto BOARD_QTMR_ENC_CHANNEL = kQTMR_Channel_0;
        constexpr auto SECONDARY_SOURCE       = kQTMR_Counter1InputPin;
        auto BOARD_QTMR_ID                    = TMR3;
        xQueueHandle gHandleIrq;
        TimerHandle_t gTimerHandle;
        uint32_t encCounter;
    } // namespace

    int init(xQueueHandle qHandle)
    {
        gHandleIrq = qHandle;
        qtmr_config_t timerCfg;
        QTMR_GetDefaultConfig(&timerCfg);
        timerCfg.primarySource   = PRIMARY_SOURCE;
        timerCfg.secondarySource = SECONDARY_SOURCE;
        QTMR_Init(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, &timerCfg);
        QTMR_StartTimer(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL, kQTMR_QuadCountMode);
        if (!gTimerHandle) {
            gTimerHandle =
                xTimerCreate("RotEncTimer", pdMS_TO_TICKS(POLL_INTERVAL_MS), true, nullptr, [](TimerHandle_t) {
                    if (gHandleIrq) {
                        uint8_t val{0x01};
                        xQueueSend(gHandleIrq, &val, 0);
                    }
                });
        }
        if (xTimerStart(gTimerHandle, 0) != pdPASS) {
            LOG_ERROR("Couldn't start encoder timer");
        }
        LOG_DEBUG("Init rotary encoder driver");
        const auto ret = (gTimerHandle && gHandleIrq) ? (kStatus_Success) : (kStatus_Fail);
        LOG_DEBUG("Init rotary encoder driver status %i", ret);
        return ret;
    }

    void deinit()
    {
        xTimerDelete(gTimerHandle, 50);
        gTimerHandle = nullptr;
        gHandleIrq   = nullptr;
        QTMR_Deinit(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL);
    }

    std::optional<type> WorkerEventHandler()
    {
        uint16_t tmp = QTMR_GetCurrentTimerCount(BOARD_QTMR_ID, BOARD_QTMR_ENC_CHANNEL);
        std::optional<type> ret;

        if (tmp != encCounter && encCounter >= 0xFFFFU - MAX_PER_100MS && encCounter <= 0xFFFFU) {
            if (tmp <= MAX_PER_100MS || tmp > encCounter) {
                ret = type::forward;
            }
            else {
                ret = type::backward;
            }
        }
        else if (tmp != encCounter && encCounter <= MAX_PER_100MS) {
            if ((tmp >= 0xFFFFU - MAX_PER_100MS) || tmp < encCounter) {
                ret = type::backward;
            }
            else {
                ret = type::forward;
            }
        }
        else if (tmp < encCounter) {
            ret = type::backward;
        }
        else if (tmp > encCounter) {
            ret = type::forward;
        }
        encCounter = tmp;

        return ret;
    }

} // namespace bsp::rotary_encoder
