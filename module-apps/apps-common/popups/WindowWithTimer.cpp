// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
                                     std::chrono::milliseconds timeout)
        : AppWindow{app, name}, timeout{timeout}
    {
        popupTimer    = app::GuiTimerFactory::createSingleShotTimer(application, this, popup::timerName, timeout);
        timerCallback = [this, name](Item &, sys::Timer &timer) {
            if (windowTimerActionCallback) {
                windowTimerActionCallback();
            }

            const auto &currentWindowName = application->getCurrentWindow()->getName();
            if (currentWindowName == name) {
                LOG_INFO("Delayed exit timer callback from '%s'", name.c_str());
                application->returnToPreviousWindow();
                return true;
            }
            LOG_WARN("Delayed exit from '%s' unsuccessful, different window '%s' displayed already",
                     name.c_str(),
                     currentWindowName.c_str());
            return false;
        };
    }

    void WindowWithTimer::resetTimer(std::chrono::seconds newTimeout)
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

    bool WindowWithTimer::onInput([[maybe_unused]] const gui::InputEvent &inputEvent)
    {
        return false;
    }

    void WindowWithTimer::setWindowTimerActionCallback(std::function<void()> &&callback)
    {
        windowTimerActionCallback = std::move(callback);
    }
} // namespace gui
