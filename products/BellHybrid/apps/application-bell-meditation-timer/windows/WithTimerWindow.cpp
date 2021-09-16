// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WithTimerWindow.hpp"

#include <GuiTimer.hpp>

namespace gui
{

    WithTimerWindow::WithTimerWindow(app::ApplicationCommon *app, std::string name) : MeditationWindow(app, move(name))
    {
        buildInterface();
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

    void WithTimerWindow::buildInterface()
    {
        MeditationWindow::buildInterface();
    }

    void WithTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        MeditationWindow::onBeforeShow(mode, data);
    }

    bool WithTimerWindow::onInput(const InputEvent &inputEvent)
    {
        return MeditationWindow::onInput(inputEvent);
    }

    void WithTimerWindow::startTimer(std::chrono::seconds secs, bool repeat)
    {
        if (repeat) {
            timer = app::GuiTimerFactory::createPeriodicTimer(
                application, this, window::timerName, std::chrono::duration_cast<std::chrono::milliseconds>(secs));
        }
        else {
            timer = app::GuiTimerFactory::createSingleShotTimer(
                application, this, window::timerName, std::chrono::duration_cast<std::chrono::milliseconds>(secs));
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
