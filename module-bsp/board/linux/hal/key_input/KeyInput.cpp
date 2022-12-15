// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <cstdio>

namespace hal::key_input
{
    std::shared_ptr<AbstractKeyInput> AbstractKeyInput::Factory::create()
    {
        return hal::impl::factory<LinuxKeyInput, AbstractKeyInput>();
    }

    void LinuxKeyInput::init(xQueueHandle qHandle)
    {
        queueHandle = qHandle;
        xTaskCreate([](void *o) { static_cast<LinuxKeyInput *>(o)->worker(); }, "keyboard", 512, this, 0, &taskHandle);
    }

    void LinuxKeyInput::deinit()
    {
        shouldRun = false;
    }

    std::vector<bsp::KeyEvent> LinuxKeyInput::getKeyEvents(KeyNotificationSource)
    {
        return keyEvents;
    }
    LinuxKeyInput::LinuxKeyInput()
    {
        keyEvents.reserve(4);
    }

    void LinuxKeyInput::worker()
    {
        constexpr auto *keyFifo           = "/tmp/myfifo3";
        constexpr auto keyFifoPermissions = 0666;
        struct pollfd pfd
        {
            .events = POLLIN
        };

        mkfifo(keyFifo, keyFifoPermissions);
        pfd.fd = open(keyFifo, O_RDONLY | O_NONBLOCK);

        while (shouldRun) {
            /// It's not possible to use POSIX blocking calls from FreeRTOS thread context. Instead, we use polling.
            if (const auto ready = poll(&pfd, 1, 0); ready > 0) {
                for (auto i = 0; i < ready; ++i) {
                    std::uint8_t buff[16]{};
                    if (const auto bytesRead = read(pfd.fd, buff, sizeof buff)) {
                        keyEvents.clear();
                        consumeBytes(buff, bytesRead);

                        std::uint8_t notification = 0x01;
                        xQueueSend(queueHandle, &notification, 100);
                    }
                }
            }
            vTaskDelay(10);
        }
        close(pfd.fd);
        vTaskDelete(nullptr);
    }

    void LinuxKeyInput::addKeyEvent(LinuxKeyInput::KeyData data)
    {
        if (const auto event = translateSliderEvent(data); event) {
            keyEvents.emplace_back(*event);
        }
    }
    void LinuxKeyInput::consumeBytes(uint8_t *buff, std::size_t size)
    {
        auto bytesLeft = size;
        while (bytesLeft > 0) {
            if (bytesLeft % keyDataSize == 0) {
                addKeyEvent(KeyData{buff[size - bytesLeft], buff[size - bytesLeft + 1]});
                bytesLeft -= keyDataSize;
            }
        }
    }
    std::optional<bsp::KeyEvent> LinuxKeyInput::translateSliderEvent(LinuxKeyInput::KeyData data)
    {
        const auto code  = static_cast<bsp::KeyCodes>(data[1]);
        const auto event = static_cast<bsp::KeyEvents>(data[0]);

        if (code == bsp::KeyCodes::SSwitchUp || code == bsp::KeyCodes::SSwitchMid ||
            code == bsp::KeyCodes::SSwitchDown) {
            if (event == bsp::KeyEvents::Pressed) {
                return bsp::KeyEvent{code, bsp::KeyEvents::Moved};
            }
            else {
                return {};
            }
        }
        return bsp::KeyEvent{code, event};
    }
} // namespace hal::key_input
