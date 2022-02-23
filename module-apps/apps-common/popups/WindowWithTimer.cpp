// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowWithTimer.hpp"
#include "ApplicationCommon.hpp"
#include "GuiTimer.hpp"

namespace gui
{
    namespace popup
    {
        constexpr auto timerName = "PopupTimer";
    } // namespace popup

    WindowWithTimer::WindowWithTimer(app::ApplicationCommon *app,
                                     const std::string &name,
                                     const std::chrono::milliseconds timeout)
        : AppWindow{app, name}, timeout{timeout}
    {
        popupTimer    = app::GuiTimerFactory::createSingleShotTimer(application, this, popup::timerName, timeout);
        timerCallback = [this, name](Item &, sys::Timer &timer) {
            if (application->getCurrentWindow()->getName() == name) {
                LOG_DEBUG("Delayed exit timer callback from: %s", name.c_str());
                application->returnToPreviousWindow();
                return true;
            }
            else {
                LOG_DEBUG("Delayed exit from: %s not succeeded, different window displayed already", name.c_str());
                return false;
            }
        };
    }

    void WindowWithTimer::resetTimer(const std::chrono::seconds newTimeout)
    {
        if (newTimeout != noTimeoutChange) {
            timeout = newTimeout;
        }
        if (!popupTimer.isValid()) {
            popupTimer = app::GuiTimerFactory::createSingleShotTimer(application, this, popup::timerName, timeout);
        }
        popupTimer.restart(timeout);
    }

    void WindowWithTimer::detachTimerIfExists()
    {
        if (popupTimer.isValid()) {
            popupTimer.stop();
            popupTimer.reset(nullptr);
        }
    }

    void WindowWithTimer::destroyInterface()
    {
        erase();
    }

    void WindowWithTimer::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        resetTimer();
    }

    void WindowWithTimer::onClose([[maybe_unused]] CloseReason reason)
    {
        detachTimerIfExists();
    }

    WindowWithTimer::~WindowWithTimer()
    {
        destroyInterface();
        detachTimerIfExists();
    }

    bool WindowWithTimer::onInput(const gui::InputEvent &inputEvent)
    {
        return false;
    }
} // namespace gui
