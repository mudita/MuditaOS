// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "Dialog.hpp"
#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/DeadBatteryWindow.hpp"
#include "windows/LogoWindow.hpp"
#include "windows/ChargingBatteryWindow.hpp"
#include "windows/Reboot.hpp"
#include "windows/Update.hpp"
#include "windows/UpdateProgress.hpp"
#include "windows/PostUpdateWindow.hpp"
#include "windows/MmiPullWindow.hpp"
#include "windows/MmiPushWindow.hpp"
#include "windows/MmiInternalMsgWindow.hpp"
#include <popups/presenter/PowerOffPresenter.hpp>
#include <popups/TetheringOffPopup.hpp>
#include <windows/Dialog.hpp>
#include <windows/DialogMetadata.hpp>
#include <messages/DialogMetadataMessage.hpp>

#include "AppWindow.hpp"
#include "data/DesktopData.hpp"

#include <service-appmgr/Controller.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <magic_enum.hpp>
#include <module-services/service-desktop/service-desktop/Constants.hpp>
#include <module-apps/messages/AppMessage.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <module-gui/gui/widgets/TopBar/SIM.hpp>
#include <service-db/DBNotificationMessage.hpp>

#include <cassert>
namespace app
{
    ApplicationDesktop::ApplicationDesktop(std::string name,
                                           std::string parent,
                                           sys::phone_modes::PhoneMode mode,
                                           StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground), AsyncCallbackReceiver(this),
          dbNotificationHandler(this)
    {
        using namespace gui::top_bar;
        topBarManager->enableIndicators({Indicator::Signal, Indicator::Time, Indicator::Battery, Indicator::SimCard});
        topBarManager->set(Indicator::SimCard,
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
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationDesktop::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        bool handled = false;
        if (auto msg = dynamic_cast<cellular::StateChange *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<sdesktop::UpdateOsMessage *>(msgl)) {
            handled = handle(msg);
        }

        // handle database response
        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                handled = true;
            }
            else if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();
                if (dbNotificationHandler.handle(result.get())) {
                    handled = true;
                    refreshMenuWindow();
                }
            }
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    auto ApplicationDesktop::handle(sdesktop::UpdateOsMessage *msg) -> bool
    {
        if (msg != nullptr && msg->messageType == updateos::UpdateMessageType::UpdateFoundOnBoot) {

            if (msg->updateStats.updateFile.has_filename()) {
                LOG_DEBUG("handle pending update found: %s", msg->updateStats.updateFile.c_str());
            }
        }

        return true;
    }

    void ApplicationDesktop::handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams)
    {
        if (auto window = getCurrentWindow()->getName();
            window == app::window::name::desktop_main_window || window == gui::popup::window::phone_lock_window) {
            switchWindow(window, std::move(notificationsParams));
        }
    }

    auto ApplicationDesktop::handle(cellular::StateChange *msg) -> bool
    {
        assert(msg);
        if (msg->request == cellular::service::State::ST::ModemFatalFailure) {
            switchWindow(app::window::name::desktop_reboot);
        }
        return false;
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        connect(sdesktop::UpdateOsMessage(), [&](sys::Message *msg) {
            auto *updateMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);
            if (updateMsg != nullptr && updateMsg->messageType == updateos::UpdateMessageType::UpdateFoundOnBoot) {

                if (getWindow(app::window::name::desktop_update)) {
                    std::unique_ptr<gui::UpdateSwitchData> data = std::make_unique<gui::UpdateSwitchData>(updateMsg);
                    switchWindow(app::window::name::desktop_update, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                }
            }

            if (updateMsg != nullptr && updateMsg->messageType == updateos::UpdateMessageType::UpdateNow) {
                auto data = std::make_unique<gui::UpdateSwitchData>(updateMsg);
                switchWindow(app::window::name::desktop_update_progress, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            }

            if (updateMsg != nullptr && updateMsg->messageType == updateos::UpdateMessageType::UpdateInform) {
                if (getWindow(app::window::name::desktop_update)) {
                    std::unique_ptr<gui::UpdateSwitchData> data = std::make_unique<gui::UpdateSwitchData>(updateMsg);
                    getWindow(app::window::name::desktop_update)->handleSwitchData(data.get());
                }
            }
            return sys::msgHandled();
        });

        connect(typeid(db::NotificationMessage), [&](sys::Message *request) {
            auto notificationMessage = static_cast<db::NotificationMessage *>(request);
            dbNotificationHandler.handle(notificationMessage);
            return sys::MessageNone{};
        });

        auto msgToSend =
            std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateCheckForUpdateOnce);
        bus.sendUnicast(msgToSend, service::name::service_desktop);

        settings->registerValueChange(
            settings::SystemProperties::osCurrentVersion,
            [this](const std::string &value) { osCurrentVersionChanged(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::osUpdateVersion,
            [this](const std::string &value) { osUpdateVersionChanged(value); },
            settings::SettingsScope::Global);

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
        windowsFactory.attach(desktop_update, [](Application *app, const std::string newname) {
            return std::make_unique<gui::UpdateWindow>(app);
        });
        windowsFactory.attach(desktop_update_progress, [](Application *app, const std::string newname) {
            return std::make_unique<gui::UpdateProgressWindow>(app);
        });
        windowsFactory.attach(desktop_post_update_window, [](Application *app, const std::string newname) {
            return std::make_unique<gui::PostUpdateWindow>(app);
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
                switchWindow(app::window::name::desktop_main_window);
            }
        }
    }

    void ApplicationDesktop::osUpdateVersionChanged(const std::string &value)
    {
        LOG_DEBUG("[ApplicationDesktop::osUpdateVersionChanged] value=%s", value.c_str());
        if (value.empty()) {
            return;
        }
        osUpdateVersion = value;
    }

    void ApplicationDesktop::osCurrentVersionChanged(const std::string &value)
    {
        LOG_DEBUG("[ApplicationDesktop::osCurrentVersionChanged] value=%s", value.c_str());
        if (value.empty()) {
            return;
        }
        osCurrentVersion = value;
    }
    void ApplicationDesktop::setOsUpdateVersion(const std::string &value)
    {
        LOG_DEBUG("[ApplicationDesktop::setOsUpdateVersion] value=%s", value.c_str());
        if (value.empty()) {
            return;
        }
        osUpdateVersion = value;
        settings->setValue(settings::SystemProperties::osUpdateVersion, value, settings::SettingsScope::Global);
    }

} // namespace app
