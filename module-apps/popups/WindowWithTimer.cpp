// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowWithTimer.hpp"
#include "Application.hpp"
#include "GuiTimer.hpp"

namespace gui
{
    namespace popup
    {
        constexpr auto timerName = "PopupTimer";
    } // namespace popup

    WindowWithTimer::WindowWithTimer(app::Application *app,
                                     const std::string &name,
                                     const std::chrono::milliseconds timeout)
        : AppWindow{app, name}
    {
        popupTimer    = app::GuiTimerFactory::createSingleShotTimer(application, this, popup::timerName, timeout);
        timerCallback = [this](Item &, sys::Timer &timer) {
            LOG_DEBUG("Delayed exit timer callback");
            application->returnToPreviousWindow();
            return true;
        };
    }

    void WindowWithTimer::resetTimer()
    {
        popupTimer.stop();
        popupTimer.start();
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
