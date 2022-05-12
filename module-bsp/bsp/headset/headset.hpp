// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "headset_key_codes.hpp"

namespace bsp {
 namespace headset {

    /**
     * @brief This method is responsible for initializing the headset controller.
     * @param qHandle key code
     */
    int32_t Init(xQueueHandle qHandle);

    /**
     * @brief This method is responsible for reading state of headset and its keys.
     * @param headsetState headset event (1 - connected, 0 - removed)
     * @param microphoneState headset event (1 - present, 0 - absent)
     * @param keyEvent key event (1 - pressed, 0 - released)
     * @param keyCode key code
     * @note Method returns `HeadsetState::Changed` if state of headset changed (connected -> removed)
     */
    HeadsetState headset_get_data(bool &headsetState, bool &microphoneState, uint8_t &keyEvent, uint8_t &keyCode);

    bool IsInserted();

    /**
     * @brief This method is responsible for deinitializing the headset controller.
     * @param qHandle key code
     */
    int32_t Deinit();

    BaseType_t headset_IRQHandler();
 }
}
