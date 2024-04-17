// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationFocusTimer.hpp"
#include "FocusCommon.hpp"

#include "windows/FocusMainWindow.hpp"
#include "windows/FocusSettingsWindow.hpp"
#include "windows/FocusTimerWindow.hpp"

#include "presenter/FocusSettingsPresenter.hpp"
#include "presenter/FocusTimerPresenter.hpp"

#include "common/models/TimeModel.hpp"
#include <common/models/AudioModel.hpp>
#include <common/windows/SessionPausedWindow.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace app
{
    ApplicationFocusTimer::ApplicationFocusTimer(std::string name,
                                                 std::string parent,
                                                 StatusIndicators statusIndicators,
                                                 StartInBackground startInBackground,
                                                 std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

    ApplicationFocusTimer::~ApplicationFocusTimer()
    {
        cpuSentinel->BlockWfiMode(false);
    }

    sys::ReturnCodes ApplicationFocusTimer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        batteryModel                 = std::make_unique<app::BatteryModel>(this);
        lowBatteryInfoModel          = std::make_unique<app::LowBatteryInfoModel>();
        cpuSentinel                  = std::make_shared<sys::CpuSentinel>(applicationFocusTimerName, this);
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);
        cpuSentinel->BlockWfiMode(true);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationFocusTimer::createUserInterface()
    {
        windowsFactory.attach(focus::window::name::main, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<focus::FocusMainWindow>(app);
        });

        windowsFactory.attach(focus::window::name::settings, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::focus::SettingsPresenter>();
            return std::make_unique<focus::SettingsWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(focus::window::name::timer, [this](ApplicationCommon *app, const std::string &name) {
            auto timeModel = std::make_unique<app::TimeModel>();
            auto presenter = std::make_unique<app::focus::FocusTimerPresenter>(
                app, settings.get(), std::move(timeModel), *batteryModel, *lowBatteryInfoModel);
            return std::make_unique<focus::FocusTimerWindow>(app, std::move(presenter));
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
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
    }

    sys::MessagePointer ApplicationFocusTimer::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }
} // namespace app
