// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "models/BatteryModel.hpp"
#include "models/TemperatureModel.hpp"
#include "presenters/HomeScreenPresenter.hpp"

#include "windows/BellBatteryShutdownWindow.hpp"
#include "windows/BellHomeScreenWindow.hpp"
#include "windows/BellMainMenuWindow.hpp"
#include "windows/BellBatteryStatusWindow.hpp"

#include <apps-common/messages/AppMessage.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/models/AlarmModel.hpp>
#include <common/models/TimeModel.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <windows/Dialog.hpp>
#include <service-appmgr/Controller.hpp>
#include <system/messages/SystemManagerMessage.hpp>
#include <common/popups/BedtimeNotificationWindow.hpp>
#include <apps-common/WindowsPopupFilter.hpp>
#include <common/windows/BellTurnOffWindow.hpp>

namespace app
{
    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             StatusIndicators statusIndicators,
                                             StartInBackground startInBackground,
                                             std::uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {

        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams &popupParams) {
            auto val = ((isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::Reboot))) ||
                        (isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::PowerOff))) ||
                        (isCurrentWindow(gui::BellTurnOffWindow::name)));
            if (val) {
                LOG_INFO("popup blocked");
                return !val;
            }
            if (not(((popupParams.getPopupId() == gui::popup::ID::AlarmActivated ||
                      popupParams.getPopupId() == gui::popup::ID::AlarmDeactivated)) and
                    (not isHomeScreenFocused()))) {
                LOG_INFO("popup blocked");
                return false;
            }
            return true;
        });

        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowAlarm, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLogoAtExit, [this](auto &&data) {
            setSystemCloseInProgress();
            switchWindow(gui::BellWelcomeWindow::defaultName);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::SystemBrownout, [this](auto &&data) {
            setSystemCloseInProgress();
            switchWindow(gui::window::name::bell_battery_shutdown, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLowBatteryScreen, [this](auto &&data) {
            /**
             * Due to the way of handling shutdown sequence by GUI renderer and applications it is required to leave
             * this handler defined but not implemented even if it's not used at all.
             * Without it, the renderer won't render the last frame properly.
             * This issue will be addressed in future PRs.
             */
            return actionHandled();
        });
    }

    sys::ReturnCodes ApplicationBellMain::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellMain::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            auto timeModel        = std::make_unique<app::TimeModel>();
            auto batteryModel     = std::make_unique<app::home_screen::BatteryModel>(app);
            auto temperatureModel = std::make_unique<app::home_screen::TemperatureModel>(app);
            auto alarmModel       = std::make_unique<app::AlarmModel>(app);
            auto presenter        = std::make_unique<app::home_screen::HomeScreenPresenter>(
                app, std::move(alarmModel), std::move(batteryModel), std::move(temperatureModel), std::move(timeModel));
            return std::make_unique<gui::BellHomeScreenWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::window::name::bell_main_menu, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellMainMenuWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bell_battery_shutdown,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellBatteryShutdownWindow>(app);
                              });

        windowsFactory.attach(gui::BellFactoryReset::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellFactoryReset>(app, std::make_unique<gui::BellPowerOffPresenter>(app));
        });

        windowsFactory.attach(gui::BellBatteryStatusWindow::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellBatteryStatusWindow>(app);
        });

        windowsFactory.attach(
            gui::window::name::bell_main_menu_dialog,
            [](ApplicationCommon *app, const std::string &name) { return std::make_unique<gui::Dialog>(app, name); });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification});
    }

    sys::MessagePointer ApplicationBellMain::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg            = Application::DataReceivedHandler(msgl);
        const auto respMessage = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (respMessage != nullptr && respMessage->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl); msg != nullptr) {
            userInterfaceDBNotification(
                msgl, [&]([[maybe_unused]] sys::Message *, [[maybe_unused]] const std::string &) { return true; });
            return sys::msgHandled();
        }
        return handleAsyncResponse(resp);
    }

    auto ApplicationBellMain::isHomeScreenFocused() -> bool
    {
        return GetName() == app::applicationBellName && getCurrentWindow()->getName() == gui::name::window::main_window;
    }

    // Empty: do not start idleTimer on application run
    void ApplicationBellMain::onStart()
    {}

    sys::MessagePointer ApplicationBellMain::handleSwitchWindow(sys::Message *msgl)
    {
        auto msg = static_cast<AppSwitchWindowMessage *>(msgl);
        if (msg) {
            const auto newWindowName = msg->getWindowName();
            if (newWindowName == gui::name::window::main_window) {
                stopIdleTimer();
            }
            else if (newWindowName == gui::window::name::bell_main_menu ||
                     newWindowName == gui::window::name::bell_main_menu_dialog) {
                startIdleTimer();
            }
        }
        return ApplicationCommon::handleSwitchWindow(msgl);
    }
} // namespace app
