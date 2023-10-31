// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellPowerNap.hpp"
#include "presenter/PowerNapMainWindowPresenter.hpp"
#include "presenter/PowerNapProgressPresenter.hpp"
#include "presenter/PowerNapSessionEndedPresenter.hpp"
#include "windows/PowerNapMainWindow.hpp"
#include "windows/PowerNapProgressWindow.hpp"
#include "windows/PowerNapSessionEndedWindow.hpp"
#include <common/models/TimeModel.hpp>
#include <Paths.hpp>
#include <common/windows/SessionPausedWindow.hpp>

namespace app
{
    ApplicationBellPowerNap::ApplicationBellPowerNap(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground,
                                                     uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)},
          frontLightModel{std::make_unique<app::bell_settings::FrontlightModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

    ApplicationBellPowerNap::~ApplicationBellPowerNap() = default;
    sys::ReturnCodes ApplicationBellPowerNap::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellPowerNap::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<powernap::PowerNapMainWindowPresenter>(app, settings.get());
            return std::make_unique<gui::PowerNapMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::powernapProgress, [this](ApplicationCommon *app, const std::string &name) {
                auto timeModel       = std::make_unique<app::TimeModel>();
                auto alarmLightOnOff = std::make_unique<bell_settings::AlarmLightOnOffModel>(this);
                auto soundsRepository =
                    std::make_unique<SoundsRepository>(paths::audio::proprietary() / paths::audio::alarm());
                auto presenter = std::make_unique<powernap::PowerNapProgressPresenter>(app,
                                                                                       settings.get(),
                                                                                       std::move(soundsRepository),
                                                                                       *audioModel,
                                                                                       *frontLightModel,
                                                                                       std::move(timeModel),
                                                                                       std::move(alarmLightOnOff));
                return std::make_unique<gui::PowerNapProgressWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::session_paused::sessionPaused,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::SessionPausedWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::powernapSessionEnded,
                              [](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<powernap::PowerNapSessionEndPresenter>(app);
                                  return std::make_unique<gui::PowerNapSessionEndedWindow>(app, std::move(presenter));
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
