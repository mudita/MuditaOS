// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellPowerNap.hpp"
#include "models/PowerNapFrontlightModel.hpp"
#include "windows/PowerNapMainWindow.hpp"
#include "windows/PowerNapProgressWindow.hpp"
#include "windows/PowerNapSessionEndedWindow.hpp"
#include "windows/PowerNapWarningWindow.hpp"
#include <common/models/TimeModel.hpp>
#include <Paths.hpp>
#include <common/windows/SessionPausedWindow.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace app
{
    ApplicationBellPowerNap::ApplicationBellPowerNap(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground,
                                                     std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

    ApplicationBellPowerNap::~ApplicationBellPowerNap()
    {
        cpuSentinel->BlockWfiMode(false);
    }

    sys::ReturnCodes ApplicationBellPowerNap::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        lowBatteryInfoModel          = std::make_unique<app::LowBatteryInfoModel>();
        cpuSentinel                  = std::make_shared<sys::CpuSentinel>(applicationBellPowerNapName, this);
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);
        cpuSentinel->BlockWfiMode(true);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellPowerNap::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window,
                              [this](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                                  auto presenter = std::make_unique<powernap::PowerNapMainWindowPresenter>(
                                      app, settings.get(), *batteryModel, *lowBatteryInfoModel);
                                  return std::make_unique<gui::PowerNapMainWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(
            gui::window::name::powerNapProgress,
            [this](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto timeModel       = std::make_unique<app::TimeModel>();
                auto frontlightModel = std::make_unique<powernap::PowerNapFrontlightModel>(this, powerNapAlarmDuration);
                auto presenter = std::make_unique<powernap::PowerNapProgressPresenter>(app,
                                                                                       settings.get(),
                                                                                       *audioModel,
                                                                                       std::move(timeModel),
                                                                                       std::move(frontlightModel),
                                                                                       powerNapAlarmDuration);
                return std::make_unique<gui::PowerNapProgressWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::session_paused::sessionPaused,
                              [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                                  return std::make_unique<gui::SessionPausedWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::powerNapSessionEnded,
                              [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                                  auto presenter = std::make_unique<powernap::PowerNapSessionEndPresenter>(app);
                                  return std::make_unique<gui::PowerNapSessionEndedWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(gui::window::name::powerNapWarning,
                              [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                                  auto presenter = std::make_unique<powernap::PowerNapWarningPresenter>(app);
                                  return std::make_unique<gui::PowerNapWarningWindow>(app, std::move(presenter));
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
    }

    sys::MessagePointer ApplicationBellPowerNap::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    void ApplicationBellPowerNap::onStop()
    {
        Application::onStop();
        audioModel->stopPlayedByThis({});
    }
} // namespace app
