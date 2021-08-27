// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyInput.hpp"

#include <hal/GenericFactory.hpp>
#include <sys/stat.h>
#include <fcntl.h>

namespace hal::key_input
{
    namespace
    {
        TaskHandle_t linux_keyboard_worker_handle = nullptr;

        std::uint8_t keyCode      = 0;
        std::uint8_t keyEventType = 0;
        int fd;

        bool handleSliderKey()
        {
            bool breakIfSliderReleased = false;
            if (keyCode == static_cast<uint8_t>(bsp::KeyCodes::SSwitchUp) ||
                keyCode == static_cast<uint8_t>(bsp::KeyCodes::SSwitchMid) ||
                keyCode == static_cast<uint8_t>(bsp::KeyCodes::SSwitchDown)) {
                if (keyEventType == static_cast<uint8_t>(bsp::KeyEvents::Pressed)) {
                    keyEventType = static_cast<uint8_t>(bsp::KeyEvents::Moved);
                }
                else {
                    breakIfSliderReleased = true;
                }
            }
            return breakIfSliderReleased;
        }

        void linux_keyboard_worker(void *pvp)
        {

            const char *myfifo = "/tmp/myfifo3";

            // Creating the named file(FIFO)
            // mkfifo(<pathname>, <permission>)
            mkfifo(myfifo, 0666);

            // Open FIFO for write only
            fd = open(myfifo, O_RDONLY | O_NONBLOCK);

            while (1) {
                std::uint8_t buff[10];
                std::int32_t readedBytes = read(fd, buff, 10);

                if (readedBytes > 1) {
                    keyEventType = buff[0];
                    keyCode      = buff[1];

                    xQueueHandle qhandle = reinterpret_cast<xQueueHandle>(pvp);

                    if (handleSliderKey()) {
                        break;
                    }
                    std::uint8_t notification = 0x01;
                    xQueueSend(qhandle, &notification, 100);
                }
                vTaskDelay(50);
            }

            close(fd);
        }
    } // namespace

    std::shared_ptr<AbstractKeyInput> AbstractKeyInput::Factory::create()
    {
        return hal::impl::factory<LinuxKeyInput, AbstractKeyInput>();
    }

    void LinuxKeyInput::init(xQueueHandle qHandle)
    {
        xTaskCreate(linux_keyboard_worker, "keyboard", 512, qHandle, 0, &linux_keyboard_worker_handle);
    }

    void LinuxKeyInput::deinit()
    {
        vTaskDelete(linux_keyboard_worker_handle);
        close(fd);
    }

    std::vector<bsp::KeyEvent> LinuxKeyInput::getKeyEvents(std::uint8_t)
    {
        using namespace bsp;
        KeyEvent keyEvent;
        keyEvent.code  = static_cast<KeyCodes>(keyCode);
        keyEvent.event = static_cast<KeyEvents>(keyEventType);
        return std::vector<KeyEvent>{keyEvent};
    }
} // namespace hal::key_input
