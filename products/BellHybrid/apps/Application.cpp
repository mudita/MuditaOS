// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
#include <service-appmgr/Constants.hpp>
#include <common/popups/BedtimeNotificationWindow.hpp>
#include <apps-common/WindowsPopupFilter.hpp>

namespace app
{
    Application::Application(std::string name,
                             std::string parent,
                             StatusIndicators statusIndicators,
                             StartInBackground startInBackground,
                             uint32_t stackDepth,
                             sys::ServicePriority priority)
        : ApplicationCommon(name, parent, statusIndicators, startInBackground, stackDepth, priority)
    {
        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams &popupParams) {
            bool val = ((isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::Reboot))) ||
                        (isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::PowerOff))) ||
                        (isCurrentWindow(gui::BellTurnOffWindow::name)));
            if (val == true) {
                LOG_ERROR("block popup - as curent window is in higher order popup");
            }
            return !val;
        });
    }

    sys::ReturnCodes Application::InitHandler()
    {
        auto ret = ApplicationCommon::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        alarmModel = std::make_unique<app::AlarmModel>(this);
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
            case ID::PowerOff:
                windowsFactory.attach(window::power_off_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BellTurnOffOptionWindow>(app, window::power_off_window);
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
} // namespace app
