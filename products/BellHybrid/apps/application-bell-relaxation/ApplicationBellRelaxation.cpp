// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellRelaxation.hpp"
#include "presenter/RelaxationMainWindowPresenter.hpp"
#include "presenter/RelaxationTimerSelectPresenter.hpp"
#include "presenter/RelaxationRunningLoopPresenter.hpp"
#include "presenter/RelaxationRunningProgressPresenter.hpp"
#include "presenter/RelaxationVolumePresenter.hpp"
#include "presenter/RelaxationPausedPresenter.hpp"
#include "presenter/RelaxationEndedPresenter.hpp"
#include "presenter/RelaxationLowBatteryPresenter.hpp"
#include "windows/RelaxationMainWindow.hpp"
#include "windows/RelaxationPausedWindow.hpp"
#include "windows/RelaxationRunningProgressWindow.hpp"
#include "windows/RelaxationRunningLoopWindow.hpp"
#include "windows/RelaxationTimerSelectWindow.hpp"
#include "windows/RelaxationVolumeWindow.hpp"
#include "windows/RelaxationEndedWindow.hpp"
#include "windows/RelaxationLowBatteryWindow.hpp"
#include "widgets/RelaxationPlayer.hpp"
#include <Paths.hpp>
#include <apps-common/messages/AppMessage.hpp>
#include <apps-common/models/SongsRepository.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/AudioModel.hpp>
#include <audio/AudioMessage.hpp>

#include <log/log.hpp>

namespace app
{
    ApplicationBellRelaxation::ApplicationBellRelaxation(std::string name,
                                                         std::string parent,
                                                         StatusIndicators statusIndicators,
                                                         StartInBackground startInBackground,
                                                         uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }
    ApplicationBellRelaxation::~ApplicationBellRelaxation() = default;

    sys::ReturnCodes ApplicationBellRelaxation::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        batteryModel = std::make_unique<app::BatteryModel>(this);
        player       = std::make_unique<relaxation::RelaxationPlayer>(*audioModel);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellRelaxation::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            auto tagsFetcher      = std::make_unique<app::music::ServiceAudioTagsFetcher>(app);
            const auto paths      = std::vector<std::string>{paths::audio::proprietary() / paths::audio::relaxation()/*,
                                                        paths::audio::userApp() / paths::audio::relaxation()*/};
            auto soundsRepository = std::make_unique<app::music::SongsRepository>(app, std::move(tagsFetcher), paths);
            auto presenter = std::make_unique<relaxation::RelaxationMainWindowPresenter>(std::move(soundsRepository));
            return std::make_unique<gui::RelaxationMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::relaxationTimerSelect, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<relaxation::RelaxationTimerSelectPresenter>(settings.get());
                return std::make_unique<gui::RelaxationTimerSelectWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::window::name::relaxationRunningProgress, [this](ApplicationCommon *app, const std::string &name) {
                auto timeModel = std::make_unique<app::TimeModel>();
                auto presenter = std::make_unique<relaxation::RelaxationRunningProgressPresenter>(
                    settings.get(), *player, std::move(timeModel));
                return std::make_unique<gui::RelaxationRunningProgressWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::window::name::relaxationRunningLoop,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto timeModel = std::make_unique<app::TimeModel>();
                                  auto presenter = std::make_unique<relaxation::RelaxationRunningLoopPresenter>(
                                      settings.get(), *player, *batteryModel, std::move(timeModel));
                                  return std::make_unique<gui::RelaxationRunningLoopWindow>(app, std::move(presenter));
                              });

        windowsFactory.attach(
            gui::window::name::relaxationPaused, [this](ApplicationCommon *app, const std::string &name) {
                auto timeModel = std::make_unique<app::TimeModel>();
                auto presenter = std::make_unique<relaxation::RelaxationPausedPresenter>(std::move(timeModel));
                return std::make_unique<gui::RelaxationPausedWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::popup::window::volume_window,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<relaxation::RelaxationVolumePresenter>(*audioModel);
                                  return std::make_unique<gui::RelaxationVolumeWindow>(app, std::move(presenter));
                              });

        windowsFactory.attach(gui::window::name::relaxationEnded, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<relaxation::RelaxationEndedPresenter>(app);
            return std::make_unique<gui::RelaxationEndedWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::window::name::relaxationLowBattery,
                              [](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<relaxation::RelaxationLowBatteryPresenter>(app);
                                  return std::make_unique<gui::RelaxationLowBatteryWindow>(app, std::move(presenter));
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification});
    }

    sys::MessagePointer ApplicationBellRelaxation::DataReceivedHandler(sys::DataMessage *msgl,
                                                                       sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    sys::MessagePointer ApplicationBellRelaxation::handleSwitchWindow(sys::Message *msgl)
    {
        if (auto msg = dynamic_cast<AppSwitchWindowMessage *>(msgl); msg) {
            const auto newWindowName = msg->getWindowName();
            if (newWindowName == gui::window::name::relaxationRunningProgress ||
                newWindowName == gui::window::name::relaxationRunningLoop ||
                newWindowName == gui::window::name::relaxationPaused) {
                stopIdleTimer();
            }
            else {
                startIdleTimer();
            }
        }
        return ApplicationCommon::handleSwitchWindow(msgl);
    }

    void ApplicationBellRelaxation::onStop()
    {
        Application::onStop();
        audioModel->stopPlayedByThis({});
    }
} // namespace app
