// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellRelaxation.hpp"
#include "presenter/RelaxationMainWindowPresenter.hpp"
#include "presenter/RelaxationTimerSelectPresenter.hpp"
#include "presenter/RelaxationProgressPresenter.hpp"
#include "presenter/RelaxationVolumePresenter.hpp"
#include "windows/RelaxationMainWindow.hpp"
#include "windows/RelaxationPausedWindow.hpp"
#include "windows/RelaxationProgressWindow.hpp"
#include "windows/RelaxationTimerSelectWindow.hpp"
#include "windows/RelaxationVolumeWindow.hpp"
#include "widgets/RelaxationPlayer.hpp"
#include <AlarmSoundPaths.hpp>
#include <apps-common/messages/AppMessage.hpp>
#include <apps-common/models/SongsRepository.hpp>
#include <common/models/TimeModel.hpp>
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
        player = std::make_unique<relaxation::RelaxationPlayer>(*audioModel);
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }
    ApplicationBellRelaxation::~ApplicationBellRelaxation() = default;

    sys::ReturnCodes ApplicationBellRelaxation::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellRelaxation::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto tagsFetcher      = std::make_unique<app::music::ServiceAudioTagsFetcher>(app);
            auto soundsRepository = std::make_unique<app::music::SongsRepository>(
                app, std::move(tagsFetcher), alarms::paths::getBackgroundSoundsDir());
            auto presenter = std::make_unique<relaxation::RelaxationMainWindowPresenter>(std::move(soundsRepository));
            return std::make_unique<gui::RelaxationMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::relaxationTimerSelect, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<relaxation::RelaxationTimerSelectPresenter>(settings.get());
                return std::make_unique<gui::RelaxationTimerSelectWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::relaxationProgress,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto timeModel = std::make_unique<app::TimeModel>();
                                  auto presenter = std::make_unique<relaxation::RelaxationProgressPresenter>(
                                      settings.get(), *player, std::move(timeModel));
                                  return std::make_unique<gui::RelaxationProgressWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(gui::window::name::relaxationPaused, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::RelaxationPausedWindow>(app);
        });

        windowsFactory.attach(gui::popup::window::volume_window,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<relaxation::RelaxationVolumePresenter>(*audioModel);
                                  return std::make_unique<gui::RelaxationVolumeWindow>(app, std::move(presenter));
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
            if (newWindowName == gui::window::name::relaxationProgress ||
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
