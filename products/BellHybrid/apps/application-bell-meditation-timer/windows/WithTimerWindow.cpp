// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WithTimerWindow.hpp"

#include <GuiTimer.hpp>

namespace gui
{

    WithTimerWindow::WithTimerWindow(app::Application *app, std::string name) : MeditationWindow(app, name)
    {
        timerCallback = [this](Item &, sys::Timer &) {
            LOG_DEBUG("timeount callback");
            onTimeout();
            return true;
        };
    }

    WithTimerWindow::~WithTimerWindow()
    {
        destroyInterface();
        detachTimerIfExists();
    }

    void WithTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        MeditationWindow::onBeforeShow(mode, data);
    }

    bool WithTimerWindow::onInput(const InputEvent &inputEvent)
    {
        return MeditationWindow::onInput(inputEvent);
    }

    void WithTimerWindow::startTimer(uint32_t secs, bool repeat)
    {
        if (repeat) {
            timer = app::GuiTimerFactory::createPeriodicTimer(
                application, this, window::timerName, std::chrono::milliseconds{secs * 1000});
        }
        else {
            timer = app::GuiTimerFactory::createSingleShotTimer(
                application, this, window::timerName, std::chrono::milliseconds{secs * 1000});
        }
        timer.start();
    }

    void WithTimerWindow::stopTimer()
    {
        if (timer.isValid()) {
            timer.stop();
            timer.reset(nullptr);
        }
    }

    void WithTimerWindow::detachTimerIfExists()
    {
        stopTimer();
    }

    void WithTimerWindow::destroyInterface()
    {
        erase();
    }

} // namespace gui
