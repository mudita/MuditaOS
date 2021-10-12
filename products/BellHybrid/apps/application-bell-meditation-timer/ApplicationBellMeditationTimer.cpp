// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "windows/IntervalChimeWindow.hpp"
#include "windows/MeditationRunningWindow.hpp"
#include "windows/MeditationTimerWindow.hpp"
#include "windows/ReadyGoingWindow.hpp"
#include "windows/SessionPausedWindow.hpp"

#include <common/models/TimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>

namespace app
{
    ApplicationBellMeditationTimer::ApplicationBellMeditationTimer(std::string name,
                                                                   std::string parent,
                                                                   StatusIndicators statusIndicators,
                                                                   StartInBackground startInBackground,
                                                                   uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
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
        windowsFactory.attach(
            gui::name::window::intervalChime, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::meditation::IntervalChimePresenter>(app, settings.get());
                return std::make_unique<gui::IntervalChimeWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::name::window::readyGoing, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::meditation::ReadyGoingPresenter>(app);
            return std::make_unique<gui::ReadyGoingWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::meditationProgress,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto timeModel = std::make_unique<app::TimeModel>();
                                  auto presenter = std::make_unique<app::meditation::MeditationProgressPresenter>(
                                      app, settings.get(), std::move(timeModel));
                                  return std::make_unique<gui::MeditationRunningWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(gui::name::window::sessionPaused, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SessionPausedWindow>(app);
        });
        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedWindow>(app, name);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification});
    }

    sys::MessagePointer ApplicationBellMeditationTimer::DataReceivedHandler(sys::DataMessage *msgl,
                                                                            sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }
} // namespace app
