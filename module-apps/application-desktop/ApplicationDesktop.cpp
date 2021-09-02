// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "ChargingBatteryWindow.hpp"
#include "DeadBatteryWindow.hpp"
#include "DesktopData.hpp"
#include "DesktopMainWindow.hpp"
#include "LogoWindow.hpp"
#include "MenuWindow.hpp"
#include "MmiInternalMsgWindow.hpp"
#include "MmiPullWindow.hpp"
#include "MmiPushWindow.hpp"
#include "Reboot.hpp"

#include <apps-common/messages/AppMessage.hpp>
#include <AppWindow.hpp>
#include <Dialog.hpp>
#include <magic_enum.hpp>
#include <messages/DialogMetadataMessage.hpp>
#include <MessageType.hpp>
#include <module-gui/gui/widgets/status-bar/SIM.hpp>
#include <module-services/service-desktop/service-desktop/Constants.hpp>
#include <popups/presenter/PowerOffPresenter.hpp>
#include <popups/TetheringOffPopup.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular-api>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <windows/Dialog.hpp>
#include <windows/DialogMetadata.hpp>

#include <cassert>

namespace app
{
    ApplicationDesktop::ApplicationDesktop(std::string name,
                                           std::string parent,
                                           sys::phone_modes::PhoneMode phoneMode,
                                           sys::bluetooth::BluetoothMode bluetoothMode,
                                           StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), phoneMode, bluetoothMode, startInBackground),
          AsyncCallbackReceiver(this), dbNotificationHandler(this)
    {
        using namespace gui::status_bar;
        statusBarManager->set(Indicator::SimCard,
                              std::make_shared<SIMConfiguration>(SIMConfiguration::DisplayMode::OnlyInactiveState));
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

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

        addActionReceiver(app::manager::actions::DisplayLowBatteryScreen, [this](auto &&data) {
            handleLowBatteryNotification(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::SystemBrownout, [this](auto &&data) {
            setSystemCloseInProgress();
            switchWindow(app::window::name::dead_battery, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayLogoAtExit, [this](auto &&data) {
            setSystemCloseInProgress();
            switchWindow(app::window::name::logo_window, std::move(data));
            return actionHandled();
        });

        connect(typeid(cellular::msg::notification::ModemStateChanged), [this](sys::Message *request) {
            auto msg = static_cast<cellular::msg::notification::ModemStateChanged *>(request);
            handle(msg);
            return sys::MessageNone{};
        });
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationDesktop::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
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
            if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();
                if (dbNotificationHandler.handle(result.get())) {
                    refreshMenuWindow();
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    void ApplicationDesktop::handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams)
    {
        if (auto window = getCurrentWindow()->getName();
            window == app::window::name::desktop_main_window || window == gui::popup::window::phone_lock_window) {
            switchWindow(window, std::move(notificationsParams));
        }
    }

    void ApplicationDesktop::handle(cellular::msg::notification::ModemStateChanged *msg)
    {
        assert(msg);
        if (msg->state == cellular::api::ModemState::Fatal) {
            switchWindow(app::window::name::desktop_reboot);
        }
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        connect(typeid(db::NotificationMessage), [&](sys::Message *request) {
            auto notificationMessage = static_cast<db::NotificationMessage *>(request);
            dbNotificationHandler.handle(notificationMessage);
            return sys::MessageNone{};
        });

        dbNotificationHandler.initHandler();

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationDesktop::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationDesktop::createUserInterface()
    {
        using namespace app::window::name;
        windowsFactory.attach(desktop_main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DesktopMainWindow>(app);
        });
        windowsFactory.attach(desktop_menu, [this](Application *app, const std::string newname) {
            return std::make_unique<gui::MenuWindow>(app, dbNotificationHandler);
        });
        windowsFactory.attach(dead_battery, [](Application *app, const std::string newname) {
            return std::make_unique<gui::DeadBatteryWindow>(app);
        });
        windowsFactory.attach(logo_window, [](Application *app, const std::string newname) {
            return std::make_unique<gui::LogoWindow>(app);
        });
        windowsFactory.attach(charging_battery, [](Application *app, const std::string newname) {
            return std::make_unique<gui::ChargingBatteryWindow>(app);
        });
        windowsFactory.attach(desktop_reboot, [](Application *app, const std::string newname) {
            auto presenter = std::make_unique<gui::PowerOffPresenter>(app);
            return std::make_unique<gui::RebootWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(desktop_mmi_pull, [](Application *app, const std::string newname) {
            return std::make_unique<gui::MmiPullWindow>(app, desktop_mmi_pull);
        });
        windowsFactory.attach(desktop_mmi_push, [](Application *app, const std::string newname) {
            return std::make_unique<gui::MmiPushWindow>(app, desktop_mmi_push);
        });
        windowsFactory.attach(desktop_mmi_internal, [](Application *app, const std::string newname) {
            return std::make_unique<gui::MmiInternalMsgWindow>(app, desktop_mmi_internal);
        });
        windowsFactory.attach(gui::window::name::dialog_confirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        windowsFactory.attach(gui::popup::window::tethering_off_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::TetheringOffPopup>(app, gui::popup::window::tethering_off_window);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock});
    }

    void ApplicationDesktop::destroyUserInterface()
    {}

    bool ApplicationDesktop::refreshMenuWindow()
    {
        if (auto menuWindow = dynamic_cast<gui::MenuWindow *>(getWindow(app::window::name::desktop_menu));
            menuWindow != nullptr) {
            menuWindow->refresh();
            return true;
        }
        return false;
    }

    void ApplicationDesktop::handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data)
    {
        auto lowBatteryState = static_cast<manager::actions::LowBatteryNotificationParams *>(data.get());
        if (lowBatteryState->isActive()) {
            if (lowBatteryState->isCharging()) {
                switchWindow(app::window::name::charging_battery, std::move(data));
            }
            else {
                switchWindow(app::window::name::dead_battery, std::move(data));
            }
        }
        else {
            auto currentWindow = getCurrentWindow();
            if (currentWindow->getName() == app::window::name::dead_battery ||
                currentWindow->getName() == app::window::name::charging_battery) {
                app::manager::Controller::sendAction(this, app::manager::actions::Home);
            }
        }
    }
} // namespace app
