// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"
#include <bsp/KeyInputCommon.hpp>

#include <hal/GenericFactory.hpp>
#include <bsp/switches/switches.hpp>
#include <bsp/rotary_encoder/rotary_encoder.hpp>
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
        bsp::rotary_encoder::init(queueHandle);
    }

    void KeyInput::deinit()
    {
        bsp::bell_switches::deinit();
        bsp::rotary_encoder::deinit();
    }

    std::vector<bsp::KeyEvent> KeyInput::getKeyEvents(KeyNotificationSource notification)
    {
        auto keyNotification = static_cast<bsp::NotificationSource>(notification);
        if (keyNotification == bsp::NotificationSource::rotaryEncoder) {
            return bsp::rotary_encoder::getKeyEvents();
        }
        else {
            return bsp::bell_switches::getKeyEvents(keyNotification);
        }
    }

    BaseType_t generalIRQHandler(std::uint32_t irqMask)
    {
        constexpr std::uint32_t encoderMask{99};
        if (irqMask == encoderMask) {
            return bsp::rotary_encoder::IRQHandler(irqMask);
        }
        else {
            return bsp::bell_switches::IRQHandler(irqMask);
        }
    }

    BaseType_t wakeupIRQHandler()
    {
        return bsp::bell_switches::wakeupIRQHandler();
    }
} // namespace hal::key_input
