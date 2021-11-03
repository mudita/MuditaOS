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

    BaseType_t EncoderIRQHandler()
    {
        return bsp::rotary_encoder::IRQHandler();
    }

    BaseType_t GPIO2SwitchesIRQHandler(std::uint32_t irqMask)
    {
        return bsp::bell_switches::GPIO2SwitchesIRQHandler(irqMask);
    }

    BaseType_t GPIO5SwitchesIRQHandler(std::uint32_t irqMask)
    {
        return bsp::bell_switches::GPIO5SwitchesIRQHandler(irqMask);
    }

} // namespace hal::key_input
