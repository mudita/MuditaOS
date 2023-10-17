// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/key_input/AbstractKeyInput.hpp>

namespace hal::key_input
{
    class LinuxKeyInput : public AbstractKeyInput
    {
        using KeyEvents                   = std::vector<bsp::KeyEvent>;
        static constexpr auto keyDataSize = 2U;
        using KeyData                     = std::array<int, keyDataSize>;

      public:
        LinuxKeyInput();
        void init(xQueueHandle qHandle) final;
        void deinit() final;
        std::vector<bsp::KeyEvent> getKeyEvents(KeyNotificationSource) final;

      private:
        void worker();

        void addKeyEvent(KeyData data);
        void consumeBytes(std::uint8_t *buff, std::size_t size);
        std::optional<bsp::KeyEvent> translateSliderEvent(KeyData key);

        bool shouldRun{true};
        KeyEvents keyEvents;
        xQueueHandle queueHandle{};
        TaskHandle_t taskHandle{};
    };
} // namespace hal::key_input
