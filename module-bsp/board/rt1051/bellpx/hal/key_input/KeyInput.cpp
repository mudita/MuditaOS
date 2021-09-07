// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>

namespace hal::key_input
{
    std::shared_ptr<AbstractKeyInput> AbstractKeyInput::Factory::create()
    {
        return hal::impl::factory<KeyInput, AbstractKeyInput>();
    }

    void KeyInput::init(xQueueHandle)
    {}

    void KeyInput::deinit()
    {}

    std::vector<bsp::KeyEvent> KeyInput::getKeyEvents(std::uint8_t)
    {
        return std::vector<bsp::KeyEvent>{};
    }

    BaseType_t generalIRQHandler()
    {
        return 0;
    }

    BaseType_t rightFunctionalIRQHandler()
    {
        return 0;
    }

} // namespace hal::key_input
