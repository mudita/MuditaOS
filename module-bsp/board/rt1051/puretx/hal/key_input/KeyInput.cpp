// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>
#include <bsp/keyboard/keyboard.hpp>

namespace hal::key_input
{
    std::shared_ptr<AbstractKeyInput> AbstractKeyInput::Factory::create()
    {
        return hal::impl::factory<KeyInput, AbstractKeyInput>();
    }

    void KeyInput::init(xQueueHandle queueHandle)
    {
        bsp::keyboard::init(queueHandle);
    }

    void KeyInput::deinit()
    {
        bsp::keyboard::deinit();
    }

    std::vector<bsp::KeyEvent> KeyInput::getKeyEvents(KeyNotificationSource notification)
    {
        return bsp::keyboard::getKeyEvents(notification);
    }

    BaseType_t generalIRQHandler(std::uint32_t irqMask)
    {
        return bsp::keyboard::IRQHandler();
    }

    BaseType_t rightFunctionalIRQHandler()
    {
        return bsp::keyboard::rightFunctionalIRQHandler();
    }

} // namespace hal::key_input
