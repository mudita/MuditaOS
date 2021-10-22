// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/key_input/KeyEventDefinitions.hpp>
#include <bsp/KeyInputCommon.hpp>

#include <cstdint>
#include <vector>

namespace bsp::bell_switches
{
    std::vector<KeyEvent> getKeyEvents(NotificationSource notification);

    std::int32_t init(xQueueHandle qHandle);

    std::int32_t deinit();

    void enableIRQ();

    void disableIRQ();

    BaseType_t IRQHandler(uint32_t mask);

    BaseType_t wakeupIRQHandler();

    bool isLatchPressed();
} // namespace bsp::bell_switches
