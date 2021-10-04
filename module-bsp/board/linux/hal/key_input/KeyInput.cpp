// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>

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
        constexpr static auto maxEvents   = 1U;
        constexpr auto *keyFifo           = "/tmp/myfifo3";
        constexpr auto keyFifoPermissions = 0666;

        struct epoll_event event
        {};
        struct epoll_event events[maxEvents];

        const auto epollFd = epoll_create1(0);
        if (epollFd == -1) {
            fprintf(stderr, "Failed to create epoll file descriptor\n");
            assert(0);
        }

        mkfifo(keyFifo, keyFifoPermissions);
        const auto fd = open(keyFifo, O_RDONLY | O_NONBLOCK);
        event.events  = EPOLLIN;
        event.data.fd = fd;

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event)) {
            fprintf(stderr, "Failed to add file descriptor to epoll\n");
            close(epollFd);
            assert(0);
        }

        while (shouldRun) {
            const auto eventCount = epoll_wait(epollFd, events, maxEvents, 1000);
            for (auto i = 0; i < eventCount; ++i) {
                std::uint8_t buff[16]{};
                const auto bytesRead = read(events[i].data.fd, buff, sizeof buff);
                if (bytesRead > 1) {
                    keyEvents.clear();
                    consumeBytes(buff, bytesRead);

                    std::uint8_t notification = 0x01;
                    xQueueSend(queueHandle, &notification, 100);
                }
            }
        }
        close(fd);
        close(epollFd);
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
