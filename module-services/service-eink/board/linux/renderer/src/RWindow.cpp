// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RWindow.hpp"
#include <module-bsp/board/linux/board.h>

#include <module-bsp/bsp/common.hpp>
#include <hal/key_input/KeyEventDefinitions.hpp>
#include <module-services/service-eink/board/linux/renderer/src/RArea.hpp>

#include <glibmm/signalproxy.h>
#include <sigc++/functors/mem_fun.h>

#include <cstddef>
#include <glib.h>
#include <map>
#include <unistd.h>
#include <utility>

static gboolean viewUpdate(gpointer data)
{
    RWindow *win = reinterpret_cast<RWindow *>(data);
    win->updateDrawBuffer();

    return true;
}

void RWindow::keyMapInit(void)
{
    keyMap.insert(std::pair<int8_t, uint32_t>('w', static_cast<uint32_t>(bsp::KeyCodes::JoystickUp)));
    keyMap.insert(std::pair<int8_t, uint32_t>('x', static_cast<uint32_t>(bsp::KeyCodes::JoystickDown)));
    keyMap.insert(std::pair<int8_t, uint32_t>('a', static_cast<uint32_t>(bsp::KeyCodes::JoystickLeft)));
    keyMap.insert(std::pair<int8_t, uint32_t>('s', static_cast<uint32_t>(bsp::KeyCodes::JoystickEnter)));
    keyMap.insert(std::pair<int8_t, uint32_t>('d', static_cast<uint32_t>(bsp::KeyCodes::JoystickRight)));

    keyMap.insert(std::pair<int8_t, uint32_t>('q', static_cast<uint32_t>(bsp::KeyCodes::FnLeft)));
    keyMap.insert(std::pair<int8_t, uint32_t>('e', static_cast<uint32_t>(bsp::KeyCodes::FnRight)));

    keyMap.insert(std::pair<int8_t, uint32_t>('0', static_cast<uint32_t>(bsp::KeyCodes::NumericKey0)));
    keyMap.insert(std::pair<int8_t, uint32_t>('1', static_cast<uint32_t>(bsp::KeyCodes::NumericKey1)));
    keyMap.insert(std::pair<int8_t, uint32_t>('2', static_cast<uint32_t>(bsp::KeyCodes::NumericKey2)));
    keyMap.insert(std::pair<int8_t, uint32_t>('3', static_cast<uint32_t>(bsp::KeyCodes::NumericKey3)));
    keyMap.insert(std::pair<int8_t, uint32_t>('4', static_cast<uint32_t>(bsp::KeyCodes::NumericKey4)));
    keyMap.insert(std::pair<int8_t, uint32_t>('5', static_cast<uint32_t>(bsp::KeyCodes::NumericKey5)));
    keyMap.insert(std::pair<int8_t, uint32_t>('6', static_cast<uint32_t>(bsp::KeyCodes::NumericKey6)));
    keyMap.insert(std::pair<int8_t, uint32_t>('7', static_cast<uint32_t>(bsp::KeyCodes::NumericKey7)));
    keyMap.insert(std::pair<int8_t, uint32_t>('8', static_cast<uint32_t>(bsp::KeyCodes::NumericKey8)));
    keyMap.insert(std::pair<int8_t, uint32_t>('9', static_cast<uint32_t>(bsp::KeyCodes::NumericKey9)));

    keyMap.insert(std::pair<int8_t, uint32_t>('z', static_cast<uint32_t>(bsp::KeyCodes::NumericKeyAst)));
    keyMap.insert(std::pair<int8_t, uint32_t>('c', static_cast<uint32_t>(bsp::KeyCodes::NumericKeyPnd)));

    keyMap.insert(std::pair<int8_t, uint32_t>('r', static_cast<uint32_t>(bsp::KeyCodes::VolUp)));
    keyMap.insert(std::pair<int8_t, uint32_t>('f', static_cast<uint32_t>(bsp::KeyCodes::VolDown)));

    keyMap.insert(std::pair<int8_t, uint32_t>('t', static_cast<uint32_t>(bsp::KeyCodes::Torch)));

    keyMap.insert(std::pair<int8_t, uint32_t>('v', static_cast<uint32_t>(bsp::KeyCodes::SSwitchDown)));
    keyMap.insert(std::pair<int8_t, uint32_t>('b', static_cast<uint32_t>(bsp::KeyCodes::SSwitchMid)));
    keyMap.insert(std::pair<int8_t, uint32_t>('n', static_cast<uint32_t>(bsp::KeyCodes::SSwitchUp)));

    keyMap.insert(std::pair<int8_t, uint32_t>('u', static_cast<uint32_t>(bsp::KeyCodes::HeadsetOk)));
    keyMap.insert(std::pair<int8_t, uint32_t>('j', static_cast<uint32_t>(bsp::KeyCodes::HeadsetVolUp)));
    keyMap.insert(std::pair<int8_t, uint32_t>('m', static_cast<uint32_t>(bsp::KeyCodes::HeadsetVolDown)));

    batteryKeyMap.insert(std::pair<int8_t, uint32_t>('[', 1));
    batteryKeyMap.insert(std::pair<int8_t, uint32_t>(']', 2));
    batteryKeyMap.insert(std::pair<int8_t, uint32_t>('p', 3));
    batteryKeyMap.insert(std::pair<int8_t, uint32_t>('l', 3));
    batteryKeyMap.insert(std::pair<int8_t, uint32_t>(';', 4));
    batteryKeyMap.insert(std::pair<int8_t, uint32_t>('\'', 5));
}
void RWindow::updateDrawBuffer()
{
    for (ssize_t i = 0; i < width * height; i++)
        for (size_t j = 0; j < 3; j++) {
            uint8_t c            = shmMemPtr[i] * 17; // scales 0-15 eink colours indices to 0-255 RGB range
            rgbMemPtr[i * 3 + j] = c;
        }
    queue_draw();
}

bool RWindow::onKeyPress(GdkEventKey *event)
{
    if (event->type == GDK_KEY_PRESS) {
        uint32_t key_code = event->keyval;

        auto f = keys.find(key_code);
        if (f == keys.end()) {

            // slider keys are not generating relase
            if ((key_code != 'v') && (key_code != 'b') && (key_code != 'n')) {
                keys.insert(std::pair<char, int>(event->keyval, event->time));
            }
            std::map<int8_t, uint32_t>::iterator key_it = keyMap.find(key_code);
            if (key_it != keyMap.end()) {
                uint8_t message[2];

                message[0] = static_cast<uint8_t>(bsp::KeyEvents::Pressed);
                message[1] = key_it->second;
                write(fifoFd, message, 2);
            }

            std::map<int8_t, uint32_t>::iterator batt_it = batteryKeyMap.find(key_code);
            if (batt_it != batteryKeyMap.end()) {
                uint8_t message = key_code;
                write(fifoFdBatt, &message, 1);
            }
        }
    }

    return false;
}

bool RWindow::onKeyRelease(GdkEventKey *event)
{
    if (event->type == GDK_KEY_RELEASE) {
        uint32_t key_code                         = event->keyval;
        std::map<uint32_t, uint32_t>::iterator it = keys.find(key_code);
        if (it != keys.end()) {

            std::map<int8_t, uint32_t>::iterator key_it = keyMap.find(key_code);
            if (key_it != keyMap.end()) {
                uint8_t message[2];

                message[0] = static_cast<uint8_t>(bsp::KeyEvents::Released);
                message[1] = key_it->second;
                write(fifoFd, message, 2);
            }

            keys.erase(it);
        }
    }

    return false;
}

RWindow::RWindow(char *shmMemory, int fifoKbd, int fifoBatt, int w, int h)
    : shmMemPtr{shmMemory}, rgbMemPtr{nullptr}, width{w}, height{h}, fifoFd{fifoKbd}, fifoFdBatt{fifoBatt}
{

    rgbMemPtr = new char[3 * w * h];
    drawArea.setDrawData(rgbMemPtr);

    set_title("GUI RENDERER");
    set_default_size(BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y);

    add(drawArea);
    show_all_children();
    g_timeout_add(50, viewUpdate, this);

    this->signal_key_press_event().connect(sigc::mem_fun(*this, &RWindow::onKeyPress), false);
    this->signal_key_release_event().connect(sigc::mem_fun(*this, &RWindow::onKeyRelease), false);

    keyMapInit();
}

RWindow::~RWindow()
{
    if (rgbMemPtr != nullptr) {
        delete[] rgbMemPtr;
    }
}
