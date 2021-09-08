// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/key_input/AbstractKeyInput.hpp>

namespace hal::key_input
{
    class KeyInput : public AbstractKeyInput
    {
      public:
        void init(xQueueHandle queueHandle) final;
        void deinit() final;
        std::vector<bsp::KeyEvent> getKeyEvents(KeyNotificationSource) final;
    };
} // namespace hal::key_input
