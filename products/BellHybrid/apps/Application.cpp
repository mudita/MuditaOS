// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <Application.hpp>

#include <audio/AudioMessage.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/popups/presenter/AlarmActivatedPresenter.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/popups/BellRebootWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <common/AudioErrorPresenter.hpp>
#include <common/windows/AudioErrorWindow.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <common/popups/BedtimeNotificationWindow.hpp>
#include <common/popups/ChargingNotificationWindow.hpp>
#include <apps-common/WindowsPopupFilter.hpp>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-time/AlarmMessage.hpp>
#include <products/BellHybrid/keymap/include/keymap/KeyMap.hpp>

namespace app
{
    Application::Application(std::string name,
                             std::string parent,
                             StatusIndicators statusIndicators,
                             StartInBackground startInBackground,
                             std::uint32_t stackDepth,
                             sys::ServicePriority priority)
        : ApplicationCommon(
              std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth, priority)
    {
        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams &popupParams) {
            const bool val = ((isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::Reboot))) ||
                              (isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::PowerOff))) ||
                              (isCurrentWindow(gui::BellTurnOffWindow::name)));
            if (val) {
                LOG_ERROR("Block popup - as current window is in higher order popup");
            }
            return val ? gui::popup::FilterType::Ignore : gui::popup::FilterType::Show;
        });

        preWakeUpModel = std::make_unique<PreWakeUpModel>(this);
        bus.channels.push_back(sys::BusChannel::AlarmNotifications);
        connect(typeid(alarms::PreWakeUpChangeState),
                [&](sys::Message *msg) { return handlePreWakeUpChangeState(msg); });
    }

    sys::ReturnCodes Application::InitHandler()
    {
        auto ret = ApplicationCommon::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        alarmModel = std::make_unique<app::AlarmModel>(this);
        alarmModel->update(nullptr);
        batteryModel = std::make_unique<app::BatteryModel>(this);
        return sys::ReturnCodes::Success;
    }

    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;

        for (auto popup : popupsList) {
            switch (popup) {
            case ID::AlarmActivated:
                windowsFactory.attach(
                    window::alarm_activated_window, [this](app::ApplicationCommon *app, const std::string &name) {
                        auto presenter = std::make_unique<app::popup::AlarmActivatedPresenter>(*alarmModel);
                        return std::make_unique<gui::AlarmActivatedWindow>(app, std::move(presenter));
                    });
                break;
            case ID::AlarmDeactivated:
                windowsFactory.attach(
                    window::alarm_deactivated_window, [this](app::ApplicationCommon *app, const std::string &name) {
                        auto presenter = std::make_unique<app::popup::AlarmActivatedPresenter>(*alarmModel);
                        return std::make_unique<gui::AlarmDeactivatedWindow>(app, std::move(presenter));
                    });
                break;
            case ID::AudioError:
                windowsFactory.attach(
                    window::audio_error_window, [](app::ApplicationCommon *app, const std::string &name) {
                        auto presenter = std::make_unique<gui::AudioErrorPresenter>(app);
                        return std::make_unique<gui::AudioErrorWindow>(app, name, std::move(presenter));
                    });
                break;
            case ID::PowerOff:
                windowsFactory.attach(window::power_off_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BellTurnOffOptionWindow>(
                        app, gui::turnOffPopupTimeout, window::power_off_window);
                });
                windowsFactory.attach(gui::BellTurnOffWindow::name,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellTurnOffWindow>(
                                              app, std::make_unique<gui::BellPowerOffPresenter>(app));
                                      });
                windowsFactory.attach(gui::BellWelcomeWindow::name,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellWelcomeWindow>(app);
                                      });
                windowsFactory.attach(gui::BellChargeWelcomeWindow::name,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellChargeWelcomeWindow>(app);
                                      });
                break;
            case ID::Reboot:
                windowsFactory.attach(window::reboot_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BellRebootWindow>(app,
                                                                   std::make_unique<gui::BellPowerOffPresenter>(app));
                });
                break;
            case ID::BedtimeNotification:
                windowsFactory.attach(window::bedtime_notification_window,
                                      [](app::ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BedtimeNotificationWindow>(app);
                                      });
                break;
            case ID::ChargingNotification:
                windowsFactory.attach(window::charging_notification_window,
                                      [](app::ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::ChargingInProgressNotificationWindow>(app);
                                      });
                break;
            case ID::ChargingDoneNotification:
                windowsFactory.attach(window::charging_done_notification_window,
                                      [](app::ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::ChargingDoneNotificationWindow>(app);
                                      });
                break;
            default:
                break;
            }
        }
    }

    std::optional<gui::popup::Blueprint> Application::popupBlueprintFallback(gui::popup::ID id)
    {
        popupBlueprint.registerBlueprint(
            id, [&](gui::popup::ID id, std::unique_ptr<gui::PopupRequestParams> &p) -> bool {
                switchWindowPopup(gui::popup::resolveWindowName(id), p->getDisposition(), nullptr, SwitchReason::Popup);
                return true;
            });
        return *popupBlueprint.getBlueprint(id);
    }

    sys::MessagePointer Application::handleKBDKeyEvent(sys::Message *msgl)
    {
        onKeyPressed();
        if (preWakeUpModel && preWakeUpModel->isActive()) {
            if (this->getState() != app::ApplicationCommon::State::ACTIVE_FORGROUND) {
                LOG_FATAL("Terrible terrible damage! Application with no focus grabbed key!");
                return sys::msgNotHandled();
            }
            const auto msg        = static_cast<sevm::KbdMessage *>(msgl);
            const auto inputEvent = keyTranslator->translate(msg->key);
            if (!inputEvent.is(gui::KeyCode::KEY_UNDEFINED) && isInputEventToHandlePreWakeUp(inputEvent)) {
                preWakeUpModel->turnOffPreWakeUp();
                return sys::msgHandled();
            }
        }
        return ApplicationCommon::handleKBDKeyEvent(msgl);
    }

    sys::MessagePointer Application::handleApplicationSwitch(sys::Message *msgl)
    {
        onStart();
        return ApplicationCommon::handleApplicationSwitch(msgl);
    }

    sys::MessagePointer Application::handleAppClose(sys::Message *msgl)
    {
        onStop();
        return ApplicationCommon::handleAppClose(msgl);
    }

    sys::MessagePointer Application::handleAppFocusLost(sys::Message *msgl)
    {
        onStop();
        return ApplicationCommon::handleAppFocusLost(msgl);
    }

    sys::MessagePointer Application::handlePreWakeUpChangeState(sys::Message *msg)
    {
        auto *message = dynamic_cast<alarms::PreWakeUpChangeState *>(msg);
        if (message == nullptr) {
            return sys::msgNotHandled();
        }
        preWakeUpModel->setActive(message->isActive());
        return sys::msgHandled();
    }

    void Application::onKeyPressed()
    {
        restartIdleTimer();
    }

    void Application::onStart()
    {
        startIdleTimer();
    }

    void Application::onStop()
    {
        stopIdleTimer();
    }

    void Application::stopAllAudio()
    {
        auto msg = std::make_shared<service::AudioStopRequest>();
        this->bus.sendUnicast(msg, service::audioServiceName);
    }

    void Application::startIdleTimer()
    {
        bus.sendUnicast(std::make_shared<StartIdleTimerMessage>(), service::name::appmgr);
    }

    void Application::restartIdleTimer()
    {
        if (idleTimerActiveFlag) {
            bus.sendUnicast(std::make_shared<RestartIdleTimerMessage>(), service::name::appmgr);
        }
    }

    void Application::stopIdleTimer()
    {
        bus.sendUnicast(std::make_shared<StopIdleTimerMessage>(), service::name::appmgr);
    }

    void Application::updateStatuses(gui::AppWindow *window) const
    {}

    void Application::resumeIdleTimer()
    {
        startIdleTimer();
        idleTimerActiveFlag = true;
    }

    void Application::suspendIdleTimer()
    {
        stopIdleTimer();
        idleTimerActiveFlag = false;
    }

    bool Application::isInputEventToHandlePreWakeUp(const gui::InputEvent inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());
        if (inputEvent.isShortRelease() && key != KeyMap::Frontlight && key != KeyMap::DeepPressDown &&
            key != KeyMap::DeepPressUp) {
            return true;
        }
        return false;
    }
} // namespace app
