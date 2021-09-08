// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>
#include <bsp/switches/switches.hpp>
#include <board/BoardDefinitions.hpp>

namespace hal::key_input
{
    std::shared_ptr<AbstractKeyInput> AbstractKeyInput::Factory::create()
    {
        return hal::impl::factory<KeyInput, AbstractKeyInput>();
    }

    void KeyInput::init(xQueueHandle queueHandle)
    {
        bsp::bell_switches::init(queueHandle);
    }

    void KeyInput::deinit()
    {
        bsp::bell_switches::deinit();
    }

    std::vector<bsp::KeyEvent> KeyInput::getKeyEvents(KeyNotificationSource notification)
    {
        return bsp::bell_switches::getKeyEvents(notification);
    }

    BaseType_t generalIRQHandler(std::uint32_t irqMask)
    {
        return bsp::bell_switches::IRQHandler(irqMask);
    }

    BaseType_t wakeupIRQHandler()
    {
        return bsp::bell_switches::wakeupIRQHandler();
    }
} // namespace hal::key_input
