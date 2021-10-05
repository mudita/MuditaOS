// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Application.hpp>

#include <common/models/AlarmModel.hpp>

#include <appmgr/messages/IdleTimerMessage.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/popups/presenter/AlarmActivatedPresenter.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <service-appmgr/Constants.hpp>

namespace app
{
    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::AlarmActivated:
                windowsFactory.attach(
                    window::alarm_activated_window, [](app::ApplicationCommon *app, const std::string &name) {
                        auto alarmModel = std::make_shared<app::AlarmModel>(app);
                        auto presenter  = std::make_unique<app::popup::AlarmActivatedPresenter>(alarmModel);
                        return std::make_unique<gui::AlarmActivatedWindow>(app, std::move(presenter));
                    });
                break;
            case ID::AlarmDeactivated:
                windowsFactory.attach(
                    window::alarm_deactivated_window, [](app::ApplicationCommon *app, const std::string &name) {
                        auto alarmModel = std::make_shared<app::AlarmModel>(app);
                        auto presenter  = std::make_unique<app::popup::AlarmActivatedPresenter>(alarmModel);
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
                windowsFactory.attach(gui::BellWelcomeWindow::defaultName,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellWelcomeWindow>(app);
                                      });
                break;
            default:
                break;
            }
        }
    }

    sys::MessagePointer Application::handleKBDKeyEvent(sys::Message *msgl)
    {
        idleTimerKeyPressedAction();
        return ApplicationCommon::handleKBDKeyEvent(msgl);
    }

    sys::MessagePointer Application::handleApplicationSwitch(sys::Message *msgl)
    {
        idleTimerApplicationStartAction();
        return ApplicationCommon::handleApplicationSwitch(msgl);
    }

    sys::MessagePointer Application::handleAppClose(sys::Message *msgl)
    {
        idleTimerApplicationStopAction();
        return ApplicationCommon::handleAppClose(msgl);
    }

    sys::MessagePointer Application::handleAppFocusLost(sys::Message *msgl)
    {
        idleTimerApplicationStopAction();
        return ApplicationCommon::handleAppFocusLost(msgl);
    }

    void Application::idleTimerKeyPressedAction()
    {
        idleTimerRestartIfActive();
    }
    void Application::idleTimerApplicationStartAction()
    {
        idleTimerRestart();
    }
    void Application::idleTimerApplicationStopAction()
    {
        idleTimerStop();
    }

    void Application::idleTimerRestartIfActive()
    {
        bus.sendUnicast(std::make_shared<IdleTimerRestartIfActive>(), service::name::appmgr);
    }
    void Application::idleTimerRestart()
    {
        bus.sendUnicast(std::make_shared<IdleTimerRestart>(), service::name::appmgr);
    }
    void Application::idleTimerStop()
    {
        bus.sendUnicast(std::make_shared<IdleTimerStop>(), service::name::appmgr);
    }

} // namespace app
