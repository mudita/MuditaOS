// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/key_input/AbstractKeyInput.hpp>

namespace hal::key_input
{
    class KeyInput : public AbstractKeyInput
    {
      public:
        void init(xQueueHandle) final;
        void deinit() final;
        std::vector<bsp::KeyEvent> getKeyEvents(KeyNotificationSource) final;
    };

    BaseType_t EncoderIRQHandler();
    BaseType_t GPIO2SwitchesIRQHandler(std::uint32_t irqMask);
    BaseType_t GPIO5SwitchesIRQHandler(std::uint32_t irqMask);

} // namespace hal::key_input
