// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "windows/MeditationMainWindow.hpp"
#include "windows/MeditationCountdownWindow.hpp"
#include "windows/MeditationRunningWindow.hpp"
#include "windows/MeditationTimerWindow.hpp"
#include "windows/SettingsWindow.hpp"
#include "windows/StatisticsWindow.hpp"

#include "models/ChimeVolume.hpp"
#include "models/StartDelay.hpp"
#include "models/ChimeInterval.hpp"

#include "presenter/SettingsPresenter.hpp"
#include "presenter/StatisticsPresenter.hpp"

#include <common/models/TimeModel.hpp>
#include <common/models/AudioModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/SessionPausedWindow.hpp>

namespace app
{
    MeditationTimer::MeditationTimer(std::string name,
                                     std::string parent,
                                     StatusIndicators statusIndicators,
                                     StartInBackground startInBackground,
                                     uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

    sys::ReturnCodes MeditationTimer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        audioModel         = std::make_unique<AudioModel>(this);
        chimeIntervalModel = std::make_unique<meditation::models::ChimeInterval>(this);
        chimeVolumeModel   = std::make_unique<meditation::models::ChimeVolume>(*audioModel);
        startDelayModel    = std::make_unique<meditation::models::StartDelay>(this);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void MeditationTimer::createUserInterface()
    {
        windowsFactory.attach(
            meditation::MeditationMainWindow::defaultName, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::meditation::MeditationTimerPresenter>(app, settings.get());
                return std::make_unique<meditation::MeditationMainWindow>(app);
            });

        windowsFactory.attach(meditation::SettingsWindow::name,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<app::meditation::SettingsPresenter>(
                                      app, *chimeIntervalModel, *chimeVolumeModel, *startDelayModel, *audioModel);
                                  return std::make_unique<meditation::SettingsWindow>(app, std::move(presenter));
                              });

        windowsFactory.attach(meditation::StatisticsWindow::name, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::meditation::StatisticsPresenter>();
            return std::make_unique<meditation::StatisticsWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(
            meditation::MeditationTimerWindow::name, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::meditation::MeditationTimerPresenter>(app, settings.get());
                return std::make_unique<meditation::MeditationTimerWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            meditation::windows::meditationCountdown, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::meditation::MeditationCountdownPresenter>(app, settings.get());
                return std::make_unique<gui::MeditationCountdownWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(meditation::windows::meditationProgress,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto timeModel = std::make_unique<app::TimeModel>();
                                  auto presenter = std::make_unique<app::meditation::MeditationProgressPresenter>(
                                      app, settings.get(), std::move(timeModel), *chimeIntervalModel);
                                  return std::make_unique<gui::MeditationRunningWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(gui::window::session_paused::sessionPaused,
                              [](ApplicationCommon *app, const std::string &name) {
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

    sys::MessagePointer MeditationTimer::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }
    MeditationTimer::~MeditationTimer() = default;
} // namespace app
