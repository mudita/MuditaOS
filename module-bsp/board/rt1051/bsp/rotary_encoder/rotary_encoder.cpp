// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/rotary_encoder/rotary_encoder.hpp"

#include <module-utils/Utils.hpp> // for byte conversion functions. it is included first because of magic enum define

#include "bsp/BoardDefinitions.hpp"

#include <fsl_common.h>
#include <timers.h>

using namespace drivers;
using namespace utils;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{
    namespace rotary_encoder
    {

        static TimerHandle_t timerHandle;
        static constexpr uint16_t ROTARY_ENCODER_POLL_INTERVAL_MS = 100;

        static void TimerHandler(TimerHandle_t xTimer)
        {
            if (qHandleIrq != nullptr) {
                uint8_t val = 0x01;
                xQueueSend(qHandleIrq, &val, 0);
            }
        }

        int32_t init(xQueueHandle qHandle)
        {
            qHandleIrq = qHandle;
            const enc_config_t encConfig;

            ENC_GetDefaultConfig(&encConfig);
            ENC_Init(SWITCHES_ENC_A_PERIPHERAL, &encConfig);
            
            ENC_DoSoftwareLoadInitialPositionValue(SWITCHES_ENC_A_PERIPHERAL);

            if (timerHandle == nullptr) {
                timerHandle = xTimerCreate(
                    "RotEncTimer", pdMS_TO_TICKS(ROTARY_ENCODER_POLL_INTERVAL_MS), true, nullptr, TimerHandler);
                if (timerHandle == nullptr) {
                    LOG_FATAL("Could not create the timer for rotary encoder polling");
                    return kStatus_Fail;
                }
            }

            xTimerStart(timerHandle, 1000);

            return kStatus_Success;
        }

        bool isPresent(void)
        {
            return true;
        }

        std::optional<bsp::KeyCodes> WorkerEventHandler()
        {
            uint16_t encDiffValue = ENC_GetHoldPositionDifferenceValue(SWITCHES_ENC_A_PERIPHERAL);
            
            // currently only single left/right keys are returned. TBD returning multiple "keypresses"
            if (encDiffValue > 0)
                return bsp::KeyCodes::JoystickRight;
            else if (endDiffValue < 0)
                return bsp::KeyCodes::JoystickLeft;
            else
                return std::nullopt;
        }

    } // namespace rotary-encoder
} // namespace bsp
