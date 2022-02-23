// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "models/BatteryModel.hpp"
#include "models/TemperatureModel.hpp"

#include "windows/BellHomeScreenWindow.hpp"
#include "windows/BellMainMenuWindow.hpp"
#include "windows/BellBatteryStatusWindow.hpp"

#include <apps-common/messages/AppMessage.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/models/AlarmModel.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <windows/Dialog.hpp>
#include <service-appmgr/Controller.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutParams.hpp>
#include <system/messages/SystemManagerMessage.hpp>
#include <common/popups/BedtimeNotificationWindow.hpp>

namespace app
{

    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             StatusIndicators statusIndicators,
                                             StartInBackground startInBackground,
                                             std::uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

        addActionReceiver(manager::actions::ShowAlarm, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLogoAtExit, [this](auto &&data) {
            requestShutdownWindow(gui::BellWelcomeWindow::defaultName);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::SystemBrownout, [this](auto &&data) {
            requestShutdownWindow(gui::popup::window::dead_battery_window);
            return actionHandled();
        });

        addActionReceiver(manager::actions::ChangeHomescreenLayout, [this](auto &&data) {
            auto msgLayout = dynamic_cast<gui::ChangeHomescreenLayoutParams *>(data.get());
            setHomeScreenLayout(msgLayout->getNewHomescreenLayoutName());
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

        auto timeModel        = std::make_unique<app::TimeModel>();
        auto batteryModel     = std::make_unique<app::home_screen::BatteryModel>(this);
        auto temperatureModel = std::make_unique<app::home_screen::TemperatureModel>(this);
        auto alarmModel       = std::make_unique<app::AlarmModel>(this);
        homeScreenPresenter   = std::make_shared<app::home_screen::HomeScreenPresenter>(
            this, std::move(alarmModel), std::move(batteryModel), std::move(temperatureModel), std::move(timeModel));

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellMain::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto layoutModel    = std::make_unique<bell_settings::LayoutModel>(app);
            auto window         = std::make_unique<gui::BellHomeScreenWindow>(app, homeScreenPresenter);
            auto selectedLayout = layoutModel->getValue();
            setHomeScreenLayout(selectedLayout);
            return window;
        });

        windowsFactory.attach(gui::window::name::bell_main_menu, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellMainMenuWindow>(app);
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

        attachPopups({
            gui::popup::ID::AlarmActivated,
            gui::popup::ID::AlarmDeactivated,
            gui::popup::ID::PowerOff,
            gui::popup::ID::Reboot,
            gui::popup::ID::BedtimeNotification,
            gui::popup::ID::DeadBatteryWindow,
        });
    }

    sys::MessagePointer ApplicationBellMain::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg            = Application::DataReceivedHandler(msgl);
        const auto respMessage = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (respMessage != nullptr && respMessage->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            for (auto &[name, window] : windowsStack.windows) {
                window->onDatabaseMessage(msg);
            }
            return sys::msgHandled();
        }

        return handleAsyncResponse(resp);
    }

    void ApplicationBellMain::showPopup(gui::popup::ID id, const gui::PopupRequestParams *params)
    {
        if (id == gui::popup::ID::AlarmActivated || id == gui::popup::ID::AlarmDeactivated) {
            if (not isHomeScreenFocused()) {
                switchWindow(gui::popup::resolveWindowName(id));
            }
        }
        else {
            Application::showPopup(id, params);
        }
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

    void ApplicationBellMain::setHomeScreenLayout(std::string layoutName)
    {
        auto homeScreenLayoutsList = gui::homeScreenLayouts();
        if (homeScreenLayoutsList.find(layoutName) == homeScreenLayoutsList.end()) {
            return;
        }
        auto layoutGenerator = homeScreenLayoutsList.at(layoutName);
        homeScreenPresenter->setLayout(layoutGenerator);
    }
} // namespace app
