// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "windows/MeditationTimerWindow.hpp"
#include "windows/IntervalChimeWindow.hpp"
#include "windows/ReadyGoingWindow.hpp"
#include "windows/MeditationRunningWindow.hpp"
#include "windows/SessionPausedWindow.hpp"
#include "windows/SessionEndWindow.hpp"

namespace app
{
    ApplicationBellMeditationTimer::ApplicationBellMeditationTimer(std::string name,
                                                                   std::string parent,
                                                                   StatusIndicators statusIndicators,
                                                                   StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground)
    {}

    ApplicationBellMeditationTimer::~ApplicationBellMeditationTimer() = default;

    sys::ReturnCodes ApplicationBellMeditationTimer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellMeditationTimer::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::meditation::MeditationTimerPresenter>(app, settings.get());
            return std::make_unique<gui::MeditationTimerWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::interval_chime, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::meditation::IntervalChimePresenter>(app);
            return std::make_unique<gui::IntervalChimeWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::ready_going, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ReadyGoingWindow>(app);
        });
        windowsFactory.attach(
            gui::name::window::meditation_running, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::meditation::MeditationProgressPresenter>(app, settings.get());
                return std::make_unique<gui::MeditationRunningWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::name::window::session_paused, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SessionPausedWindow>(app);
        });
        windowsFactory.attach(gui::name::window::session_end, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SessionEndWindow>(app);
        });
    }

    sys::MessagePointer ApplicationBellMeditationTimer::DataReceivedHandler(sys::DataMessage *msgl,
                                                                            sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
} // namespace app
