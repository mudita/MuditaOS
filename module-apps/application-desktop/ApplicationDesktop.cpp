// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "ChargingBatteryWindow.hpp"
#include "DeadBatteryWindow.hpp"
#include "DeadBatteryInfoWindow.hpp"
#include "DesktopMainWindow.hpp"
#include "ClosingWindow.hpp"
#include "MenuWindow.hpp"
#include "MmiInternalMsgWindow.hpp"
#include "MmiPullWindow.hpp"
#include "WindowsPopupFilter.hpp"
#include "MmiConfirmationWindow.hpp"

#include <apps-common/messages/AppMessage.hpp>
#include <module-gui/gui/widgets/status-bar/SIM.hpp>
#include <popups/TetheringOffPopup.hpp>
#include <popups/TetheringMenuPopup.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <system/messages/SystemManagerMessage.hpp>

namespace app
{
    ApplicationDesktop::ApplicationDesktop(std::string name,
                                           std::string parent,
                                           StatusIndicators statusIndicators,
                                           StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground),
          AsyncCallbackReceiver(this), dbNotificationHandler(this)
    {
        using namespace gui::status_bar;
        statusBarManager->set(Indicator::SimCard,
                              std::make_shared<SIMConfiguration>(SIMConfiguration::DisplayMode::OnlyInactiveState));
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams & /*popupParams*/) {
            return blockAllPopups ? gui::popup::FilterType::Ignore : gui::popup::FilterType::Show;
        });

        addActionReceiver(app::manager::actions::ShowMMIResponse, [this](auto &&data) {
            switchWindow(app::window::name::desktop_mmi_pull, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::ShowMMIPush, [this](auto &&data) {
            switchWindow(app::window::name::desktop_mmi_push, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::ShowMMIResult, [this](auto &&data) {
            switchWindow(app::window::name::desktop_mmi_internal, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::ShowMMIConfirmation, [this](auto &&data) {
            switchWindow(app::window::name::desktop_mmi_confirmation, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLowBatteryScreen, [this](auto &&data) {
            handleLowBatteryNotification(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::SystemBrownout, [this](auto &&data) {
            requestShutdownWindow(app::window::name::dead_battery);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLogoAtExit, [this](auto &&data) {
            requestShutdownWindow(app::window::name::closing_window);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayChargeAtExit, [this](auto &&data) {
            requestShutdownWindow(app::window::name::closing_window);
            return actionHandled();
        });
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationDesktop::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        const auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    sys::MessagePointer ApplicationDesktop::handleAsyncResponse(sys::ResponseMessage *resp)
    {
        Application::handleAsyncResponse(resp);

        if (resp != nullptr) {
            if (const auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                const auto result = msg->getResult();
                if (dbNotificationHandler.handle(result.get())) {
                    refreshMenuWindow();
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    void ApplicationDesktop::handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams)
    {
        if (const auto window = getCurrentWindow()->getName();
            (window == app::window::name::desktop_main_window) || (window == gui::popup::window::phone_lock_window)) {
            const auto refreshMode = getRefreshModeFromNotifications(notificationsParams.get());
            updateCurrentWindow(std::move(notificationsParams), gui::ShowMode::GUI_SHOW_INIT, refreshMode);
        }
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        connect(typeid(db::NotificationMessage), [&](sys::Message *request) {
            const auto notificationMessage = static_cast<db::NotificationMessage *>(request);
            dbNotificationHandler.handle(notificationMessage);
            return sys::MessageNone{};
        });

        dbNotificationHandler.initHandler();

        return sys::ReturnCodes::Success;
    }

    void ApplicationDesktop::createUserInterface()
    {
        using namespace app::window::name;
        using namespace gui::popup::window;

        windowsFactory.attach(desktop_main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DesktopMainWindow>(app);
        });
        windowsFactory.attach(desktop_menu, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MenuWindow>(app, dbNotificationHandler);
        });
        windowsFactory.attach(closing_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ClosingWindow>(app);
        });
        windowsFactory.attach(dead_battery, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DeadBatteryWindow>(app, name);
        });
        windowsFactory.attach(charging_battery, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ChargingBatteryWindow>(app, name);
        });
        windowsFactory.attach(dead_battery_info, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DeadBatteryInfoWindow>(app, name);
        });
        windowsFactory.attach(desktop_mmi_pull, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MmiPullWindow>(app, name);
        });
        windowsFactory.attach(desktop_mmi_push, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MmiPushWindow>(app, name);
        });
        windowsFactory.attach(desktop_mmi_internal, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MmiInternalMsgWindow>(app, name);
        });
        windowsFactory.attach(tethering_menu_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::TetheringMenuPopup>(app, name);
        });
        windowsFactory.attach(tethering_off_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::TetheringOffPopup>(app, name);
        });
        windowsFactory.attach(desktop_mmi_confirmation, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MmiConfirmationWindow>(app, name);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::BluetoothAuthenticate,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationDesktop::destroyUserInterface()
    {}

    bool ApplicationDesktop::refreshMenuWindow()
    {
        if (const auto menuWindow = dynamic_cast<gui::MenuWindow *>(getWindow(app::window::name::desktop_menu));
            menuWindow != nullptr) {
            menuWindow->refresh();
            return true;
        }
        return false;
    }

    void ApplicationDesktop::handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data)
    {
        const auto lowBatteryState = static_cast<manager::actions::LowBatteryNotificationParams *>(data.get());
        blockAllPopups       = lowBatteryState->isActive();
        const auto currentWindow   = getCurrentWindow();
        if (currentWindow->getName() == app::window::name::dead_battery ||
            currentWindow->getName() == app::window::name::charging_battery) {
            data->ignoreCurrentWindowOnStack = true;
        }

        if (blockAllPopups) {
            if (lowBatteryState->isCharging()) {
                switchWindow(app::window::name::charging_battery, std::move(data));
            }
            else {
                switchWindow(app::window::name::dead_battery, std::move(data));
            }
        }
        else {
            app::manager::Controller::sendAction(this, app::manager::actions::Home, std::move(data));
        }
    }
} // namespace app
