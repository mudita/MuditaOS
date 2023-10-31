// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "models/TemperatureModel.hpp"

#include "windows/BellBatteryShutdownWindow.hpp"
#include "windows/BellHomeScreenWindow.hpp"
#include "windows/BellMainMenuWindow.hpp"
#include "windows/BellBatteryStatusWindow.hpp"

#include <apps-common/messages/AppMessage.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <windows/Dialog.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutParams.hpp>
#include <system/messages/SystemManagerMessage.hpp>
#include <apps-common/WindowsPopupFilter.hpp>
#include <WindowsStack.hpp>
#include <popups/Popups.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <appmgr/messages/AlarmMessage.hpp>

namespace app
{

    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             StatusIndicators statusIndicators,
                                             StartInBackground startInBackground,
                                             std::uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {
        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams &params) {
            const auto popupId = params.getPopupId();
            if (popupId == gui::popup::ID::Alarm || popupId == gui::popup::ID::AlarmActivated ||
                popupId == gui::popup::ID::AlarmDeactivated) {
                return gui::name::window::main_window != getCurrentWindow()->getName() ? gui::popup::FilterType::Show
                                                                                       : gui::popup::FilterType::Remove;
            }
            return gui::popup::FilterType::Show;
        });

        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        bus.channels.push_back(sys::BusChannel::USBNotifications);
        bus.channels.push_back(sys::BusChannel::AlarmNotifications);

        addActionReceiver(manager::actions::ShowAlarm, [this](auto &&data) {
            clearPendingPopups();
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });

        addActionReceiver(manager::actions::ChangeHomescreenLayout, [this](auto &&data) {
            auto msgLayout = dynamic_cast<gui::ChangeHomescreenLayoutParams *>(data.get());
            setHomeScreenLayout(msgLayout->getNewHomescreenLayoutName());
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLogoAtExit, [this](auto &&data) {
            requestShutdownWindow(gui::BellWelcomeWindow::name);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayChargeAtExit, [this](auto &&data) {
            requestShutdownWindow(gui::BellChargeWelcomeWindow::name);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::SystemBrownout, [this](auto &&data) {
            requestShutdownWindow(gui::window::name::bell_battery_shutdown);
            return actionHandled();
        });

        connect(typeid(sdesktop::usb::USBConfigured), [&](sys::Message *msg) -> sys::MessagePointer {
            if (getCurrentWindow()->getName() == gui::name::window::main_window) {
                homeScreenPresenter->setUSBStatusConnected();
            }
            return sys::msgHandled();
        });
        connect(typeid(AlarmDeactivated), [this](sys::Message *request) -> sys::MessagePointer {
            alarmModel->turnOff();
            alarmModel->activateAlarm(false);
            return sys::msgHandled();
        });
        connect(typeid(AlarmActivated), [this](sys::Message *request) -> sys::MessagePointer {
            alarmModel->activateAlarm(true);
            return sys::msgHandled();
        });
    }

    sys::ReturnCodes ApplicationBellMain::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        timeModel           = std::make_unique<app::TimeModel>();
        batteryModel        = std::make_unique<app::BatteryModel>(this);
        temperatureModel    = std::make_unique<app::home_screen::TemperatureModel>(this);
        homeScreenPresenter = std::make_shared<app::home_screen::HomeScreenPresenter>(
            this, *alarmModel, *batteryModel, *temperatureModel, *timeModel);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellMain::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto layoutModel = std::make_unique<bell_settings::LayoutModel>(app);
            auto window      = std::make_unique<gui::BellHomeScreenWindow>(app, homeScreenPresenter);
            if (const auto selectedLayout = layoutModel->getValue();
                not selectedLayout or not setHomeScreenLayout(*selectedLayout)) {

                const auto defaultLayout = gui::factory::getDefaultLayout();

                /// Update database entry even if such a field does not exist
                layoutModel->setValue(defaultLayout);
                setHomeScreenLayout(defaultLayout);

                LOG_WARN("Invalid layout, loading the default one, %s", defaultLayout.c_str());
            }

            return window;
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
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
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

    // Empty: do not start idleTimer on application run
    void ApplicationBellMain::onStart()
    {}

    sys::MessagePointer ApplicationBellMain::handleSwitchWindow(sys::Message *msgl)
    {
        auto msg = static_cast<AppSwitchWindowMessage *>(msgl);
        if (msg != nullptr) {
            const auto newWindowName = msg->getWindowName();
            if (newWindowName == gui::name::window::main_window) {
                stopIdleTimer();
            }
            else if (newWindowName == gui::window::name::bell_main_menu ||
                     newWindowName == gui::window::name::bell_main_menu_dialog ||
                     newWindowName == gui::BellBatteryStatusWindow::name) {
                startIdleTimer();
                clearPendingPopups();
            }
        }
        return ApplicationCommon::handleSwitchWindow(msgl);
    }

    bool ApplicationBellMain::setHomeScreenLayout(std::string layoutName)
    {
        auto homeScreenLayoutsList = gui::factory::getAllLayouts();
        if (homeScreenLayoutsList.find(layoutName) == homeScreenLayoutsList.end()) {
            return false;
        }
        const auto layoutGenerator = homeScreenLayoutsList.at(layoutName);
        homeScreenPresenter->setLayout(layoutGenerator);
        return true;
    }

    ApplicationBellMain::~ApplicationBellMain() = default;
} // namespace app
