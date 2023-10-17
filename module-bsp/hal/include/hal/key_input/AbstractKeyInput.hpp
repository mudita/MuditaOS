// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "KeyEventDefinitions.hpp"

#include <FreeRTOS.h>
#include <queue.h>

#include <memory>
#include <vector>

namespace hal::key_input
{
    class AbstractKeyInput
    {
      public:
        struct Factory
        {
            static std::shared_ptr<AbstractKeyInput> create();
        };

        virtual ~AbstractKeyInput() = default;

        virtual void init(xQueueHandle)                                        = 0;
        virtual void deinit()                                                  = 0;
        virtual std::vector<bsp::KeyEvent> getKeyEvents(KeyNotificationSource) = 0;
    };

    BaseType_t generalIRQHandler(std::uint32_t irqMask);
    BaseType_t rightFunctionalIRQHandler();
    BaseType_t wakeupIRQHandler();
} // namespace hal::key_input
