// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RArea.hpp"

#include <hal/key_input/KeyEventDefinitions.hpp>

#include <gdk/gdk.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>

#include <cstdint>
#include <map>

class RWindow : public Gtk::Window
{
  protected:
    RArea drawArea;
    // pointer to shared memory with gui renderer buffer passed from main.
    char *shmMemPtr;
    // local buffer
    char *rgbMemPtr;
    // dimensions of the shared memory buffer
    int width, height;
    // map that holds pair of key, time
    //	pair is inserted into map when there is key event of GDK_KEY_PRESS
    // it is removed when there is GDK_KEY_RELEASE event
    std::map<uint32_t, uint32_t> keys;

    std::map<int8_t, uint32_t> keyMap;
    std::map<int8_t, uint32_t> batteryKeyMap;
    // named pipe file descriptor
    int fifoFd;
    // named pipe file descriptor
    int fifoFdBatt;
    void keyMapInit(void);

  public:
    RWindow(char *shmMemory, int fifoKbd, int fifoBatt, int w, int h);
    virtual ~RWindow();
    bool onKeyPress(GdkEventKey *event);
    bool onKeyRelease(GdkEventKey *event);

    void updateDrawBuffer();
};
