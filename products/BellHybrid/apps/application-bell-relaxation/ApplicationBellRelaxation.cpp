// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellRelaxation.hpp"
#include "presenter/RelaxationMainWindowPresenter.hpp"
#include "presenter/RelaxationTimerSelectPresenter.hpp"
#include "presenter/RelaxationRunningLoopPresenter.hpp"
#include "presenter/RelaxationRunningProgressPresenter.hpp"
#include "presenter/RelaxationVolumePresenter.hpp"
#include "presenter/RelaxationPausedPresenter.hpp"
#include "presenter/RelaxationEndedPresenter.hpp"
#include "windows/RelaxationMainWindow.hpp"
#include "windows/RelaxationPausedWindow.hpp"
#include "windows/RelaxationRunningProgressWindow.hpp"
#include "windows/RelaxationRunningLoopWindow.hpp"
#include "windows/RelaxationTimerSelectWindow.hpp"
#include "windows/RelaxationVolumeWindow.hpp"
#include "windows/RelaxationEndedWindow.hpp"
#include "widgets/RelaxationPlayer.hpp"
#include <Paths.hpp>
#include <apps-common/messages/AppMessage.hpp>
#include <apps-common/models/SongsRepository.hpp>
#include <common/SoundsRepository.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/AudioModel.hpp>
#include <common/models/SongsModel.hpp>
#include <common/models/RelaxationFadeModel.hpp>
#include <common/windows/AppsBatteryStatusWindow.hpp>
#include <common/AudioErrorPresenter.hpp>
#include <common/windows/AudioErrorWindow.hpp>
#include <common/BellCommonNames.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace
{
    constexpr auto relaxationRebuildTimer         = "RelaxationRebuildTimer";
    constexpr auto relaxationRebuildTimerInterval = std::chrono::milliseconds{5000};
} // namespace

namespace app
{
    ApplicationBellRelaxation::ApplicationBellRelaxation(std::string name,
                                                         std::string parent,
                                                         StatusIndicators statusIndicators,
                                                         StartInBackground startInBackground,
                                                         std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    ApplicationBellRelaxation::~ApplicationBellRelaxation()
    {
        cpuSentinel->BlockWfiMode(false);
    }

    sys::ReturnCodes ApplicationBellRelaxation::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        cpuSentinel                  = std::make_shared<sys::CpuSentinel>(applicationBellRelaxationName, this);
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);
        cpuSentinel->BlockWfiMode(true);

        batteryModel                 = std::make_unique<app::BatteryModel>(this);
        lowBatteryInfoModel          = std::make_unique<app::LowBatteryInfoModel>();
        fadeModel                    = std::make_unique<app::RelaxationFadeModel>(this);
        player                       = std::make_unique<relaxation::RelaxationPlayer>(*fadeModel, *audioModel);
        relaxationRebuildTimerHandle = sys::TimerFactory::createSingleShotTimer(
            this, relaxationRebuildTimer, relaxationRebuildTimerInterval, [this](sys::Timer &) {
                const auto mainWindow = getWindow(gui::name::window::main_window);
                if (mainWindow != nullptr) {
                    mainWindow->rebuild();
                }
            });

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellRelaxation::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            const app::LabelsWithPaths labelsWithPaths{
                {"app_bell_relaxation_sounds", paths::audio::proprietary() / paths::audio::relaxation()},
                {"app_bell_relaxation_noises", paths::audio::proprietary() / paths::audio::colorOfNoises()},
                {"app_bell_relaxation_uploaded_sounds", paths::audio::userApp() / paths::audio::relaxation()}};
            const auto pathsSortingVector = std::vector<SoundsRepository::PathSorting>{
                {paths::audio::proprietary() / paths::audio::relaxation(), SoundsRepository::SortingBy::TitleAscending},
                {paths::audio::proprietary() / paths::audio::colorOfNoises(),
                 SoundsRepository::SortingBy::TitleAscending},
                {paths::audio::userApp() / paths::audio::relaxation(), SoundsRepository::SortingBy::TitleAscending}};

            auto soundsRepository = std::make_unique<SoundsRepository>(app, pathsSortingVector);
            auto songsModel = std::make_unique<app::SongsModel>(app, std::move(soundsRepository), labelsWithPaths);
            auto presenter  = std::make_unique<relaxation::RelaxationMainWindowPresenter>(std::move(songsModel));
            return std::make_unique<gui::RelaxationMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::relaxationTimerSelect,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<relaxation::RelaxationTimerSelectPresenter>(
                                      settings.get(), *batteryModel, *lowBatteryInfoModel);
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

        windowsFactory.attach(gui::window::name::relaxationPaused, [](ApplicationCommon *app, const std::string &name) {
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
                                  return std::make_unique<gui::AppsBatteryStatusWindow>(app, name);
                              });

        windowsFactory.attach(gui::window::name::audioErrorWindow,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  auto presenter      = std::make_unique<gui::AudioErrorPresenter>(app);
                                  auto onExitCallback = [this]() { switchWindow(gui::name::window::main_window); };
                                  return std::make_unique<gui::AudioErrorWindow>(
                                      app, name, std::move(presenter), std::move(onExitCallback));
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
    }

    sys::MessagePointer ApplicationBellRelaxation::DataReceivedHandler(sys::DataMessage *msgl,
                                                                       sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl); msg != nullptr) {
            if (msg->interface == db::Interface::Name::MultimediaFiles && msg->type != db::Query::Type::Read) {
                if (!relaxationRebuildTimerHandle.isActive()) {
                    relaxationRebuildTimerHandle.start();
                }
                else {
                    relaxationRebuildTimerHandle.restart(relaxationRebuildTimerInterval);
                }
                return sys::msgHandled();
            }
            userInterfaceDBNotification(
                msgl,
                []([[maybe_unused]] sys::Message *msg, [[maybe_unused]] const std::string &name) { return true; });
            return sys::msgHandled();
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
