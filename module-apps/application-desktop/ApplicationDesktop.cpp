// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "Dialog.hpp"
#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/PinLockWindow.hpp"
#include "windows/PowerOffWindow.hpp"
#include "windows/DeadBatteryWindow.hpp"
#include "windows/LogoWindow.hpp"
#include "windows/ChargingBatteryWindow.hpp"
#include "windows/LockedInfoWindow.hpp"
#include "windows/Reboot.hpp"
#include "windows/Update.hpp"
#include "windows/UpdateProgress.hpp"
#include "windows/PostUpdateWindow.hpp"
#include "windows/MmiPullWindow.hpp"
#include "windows/MmiPushWindow.hpp"
#include "windows/MmiInternalMsgWindow.hpp"
#include "presenter/PowerOffPresenter.hpp"

#include "AppWindow.hpp"
#include "data/LockPhoneData.hpp"

#include <service-db/DBServiceAPI.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <service-db/QueryMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/magic_enum/include/magic_enum.hpp>
#include <module-apps/messages/AppMessage.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>

#include <cassert>
namespace app
{
    ApplicationDesktop::ApplicationDesktop(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground), lockHandler(this)
    {
        using namespace gui::top_bar;
        topBarManager->enableIndicators({Indicator::Signal,
                                         Indicator::Time,
                                         Indicator::Battery,
                                         Indicator::SimCard,
                                         Indicator::NetworkAccessTechnology});
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

        addActionReceiver(app::manager::actions::RequestPin, [this](auto &&data) {
            lockHandler.handlePasscodeRequest(gui::PinLock::LockType::SimPin, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestScreenPasscode, [this](auto &&data) {
            lockHandler.handleScreenPasscodeRequest(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPuk, [this](auto &&data) {
            lockHandler.handlePasscodeRequest(gui::PinLock::LockType::SimPuk, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPinChange, [this](auto &&data) {
            lockHandler.handlePinChangeRequest(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::BlockSim, [this](auto &&data) {
            lockHandler.handleSimBlocked(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::UnlockSim, [this](auto &&data) {
            lockHandler.handleUnlockSim(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::DisplayCMEError, [this](auto &&data) {
            lockHandler.handleCMEError(std::move(data));
            return actionHandled();
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

        addActionReceiver(app::manager::actions::AutoLock, [this](auto &&data) {
            if (lockHandler.isScreenLocked()) {
                return actionHandled();
            }
            if (this->getState() == app::Application::State::ACTIVE_FORGROUND) {
                ///> we cannot block on all windows
                if (getCurrentWindow()->getName() == gui::name::window::main_window ||
                    getCurrentWindow()->getName() == app::window::name::desktop_menu) {
                    lockHandler.lockScreen();
                    switchWindow(app::window::name::desktop_main_window, std::move(data));
                }
            }
            else {
                lockHandler.lockScreen();
                switchWindow(app::window::name::desktop_main_window, std::move(data));
            }

            return actionHandled();
        });
    }
    ApplicationDesktop::~ApplicationDesktop()
    {
        LOG_INFO("Desktop destruktor");
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
        if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<cellular::StateChange *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<sdesktop::developerMode::DeveloperModeRequest *>(msgl)) {
            if (auto event = dynamic_cast<sdesktop::developerMode::ScreenlockCheckEvent *>(msg->event.get())) {
                handled = handle(event);
            }
        }
        else if (auto msg = dynamic_cast<sdesktop::UpdateOsMessage *>(msgl)) {
            handled = handle(msg);
        }

        // handle database response
        if (resp != nullptr) {
            if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();
                if (auto response = dynamic_cast<db::query::notifications::GetAllResult *>(result.get())) {
                    handled = handle(response);
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

    auto ApplicationDesktop::handle(sdesktop::developerMode::ScreenlockCheckEvent *event) -> bool
    {
        if (event != nullptr) {
            auto event = std::make_unique<sdesktop::developerMode::ScreenlockCheckEvent>(lockHandler.isScreenLocked());
            auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
            bus.sendUnicast(std::move(msg), service::name::service_desktop);
        }

        return true;
    }

    auto ApplicationDesktop::handle(db::query::notifications::GetAllResult *msg) -> bool
    {
        assert(msg);
        auto records = *msg->getResult();

        bool rebuildMainWindow = false;

        for (auto record : records) {
            switch (record.key) {
            case NotificationsRecord::Key::Calls:
                rebuildMainWindow |= record.value != notifications.notSeen.Calls;
                notifications.notSeen.Calls = record.value;
                break;

            case NotificationsRecord::Key::Sms:
                rebuildMainWindow |= record.value != notifications.notSeen.SMS;
                notifications.notSeen.SMS = record.value;
                break;

            case NotificationsRecord::Key::NotValidKey:
            case NotificationsRecord::Key::NumberOfKeys:
                LOG_ERROR("Not a valid key");
                return false;
            }
        }

        auto ptr = getCurrentWindow();
        if (rebuildMainWindow && ptr->getName() == app::window::name::desktop_main_window) {
            ptr->rebuild();
        }

        return true;
    }

    auto ApplicationDesktop::handle(db::NotificationMessage *msg) -> bool
    {
        assert(msg);

        if (msg->interface == db::Interface::Name::Notifications && msg->type == db::Query::Type::Update) {
            return requestNotSeenNotifications();
        }

        if ((msg->interface == db::Interface::Name::Calllog || msg->interface == db::Interface::Name::SMSThread ||
             msg->interface == db::Interface::Name::SMS) &&
            msg->type != db::Query::Type::Read) {
            requestNotReadNotifications();
            if (auto menuWindow = dynamic_cast<gui::MenuWindow *>(getWindow(app::window::name::desktop_menu));
                menuWindow != nullptr) {
                menuWindow->refresh();
                return true;
            }
        }

        return false;
    }

    auto ApplicationDesktop::handle(cellular::StateChange *msg) -> bool
    {
        assert(msg);
        if (msg->request == cellular::State::ST::URCReady) {
            if (need_sim_select && !lockHandler.isScreenLocked()) {
                manager::Controller::sendAction(this, manager::actions::SelectSimCard);
                return true;
            }
            else if (need_sim_select == false) {
                bus.sendUnicast(std::make_shared<CellularSimProcedureMessage>(), ServiceCellular::serviceName);
            }
        }
        if (msg->request == cellular::State::ST::ModemFatalFailure) {
            switchWindow(app::window::name::desktop_reboot);
        }
        return false;
    }

    bool ApplicationDesktop::showCalls()
    {
        LOG_DEBUG("show calls!");
        return manager::Controller::sendAction(this, manager::actions::ShowCallLog);
    }

    bool ApplicationDesktop::clearCallsNotification()
    {
        LOG_DEBUG("Clear Call notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
        notifications.notSeen.Calls = 0;
        return true;
    }

    bool ApplicationDesktop::clearMessagesNotification()
    {
        LOG_DEBUG("Clear Sms notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
        notifications.notSeen.SMS = 0;
        return true;
    }

    bool ApplicationDesktop::requestNotSeenNotifications()
    {
        const auto [succeed, _] = DBServiceAPI::GetQuery(
            this, db::Interface::Name::Notifications, std::make_unique<db::query::notifications::GetAll>());
        return succeed;
    }

    bool ApplicationDesktop::requestNotReadNotifications()
    {
        notifications.notRead.Calls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
        notifications.notRead.SMS   = DBServiceAPI::ThreadGetCount(this, EntryState::UNREAD);
        return true;
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        requestNotReadNotifications();
        requestNotSeenNotifications();

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);

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
            return msgHandled();
        });

        auto msgToSend =
            std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateCheckForUpdateOnce);
        bus.sendUnicast(msgToSend, service::name::service_desktop);

        //        settings->registerValueChange(
        //            settings::SystemProperties::activeSim,
        //            [this](std::string value) { activeSimChanged(value); },
        //            settings::SettingsScope::Global);
        //        Store::GSM::get()->selected = Store::GSM::SIM::NONE;
        //        settings->registerValueChange(
        //            settings::SystemProperties::lockPassHash,
        //            [this](std::string value) { lockPassHashChanged(value); },
        //            settings::SettingsScope::Global);
        //
        //        settings->registerValueChange(
        //            settings::SystemProperties::osCurrentVersion,
        //            [this](std::string value) { osCurrentVersionChanged(std::move(value)); },
        //            settings::SettingsScope::Global);
        //
        //        settings->registerValueChange(
        //            settings::SystemProperties::osUpdateVersion,
        //            [this](std::string value) { osUpdateVersionChanged(std::move(value)); },
        //            settings::SettingsScope::Global);

        LOG_ERROR(
            "hmm ale jak ? %s",
            settings->getValue(settings::SystemProperties::lockPassHash, settings::SettingsScope::Global).c_str());
        lockPassHashChanged(
            settings->getValue(settings::SystemProperties::lockPassHash, settings::SettingsScope::Global));

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationDesktop::DeinitHandler()
    {
        LOG_INFO("DeinitHandler");
        settings->unregisterValueChange();
        return sys::ReturnCodes::Success;
    }

    void ApplicationDesktop::createUserInterface()
    {
        using namespace app::window::name;
        windowsFactory.attach(desktop_main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DesktopMainWindow>(app);
        });
        windowsFactory.attach(desktop_pin_lock, [&](Application *app, const std::string newname) {
            return std::make_unique<gui::PinLockWindow>(app, desktop_pin_lock);
        });
        windowsFactory.attach(desktop_menu, [](Application *app, const std::string newname) {
            return std::make_unique<gui::MenuWindow>(app);
        });
        windowsFactory.attach(desktop_poweroff, [](Application *app, const std::string newname) {
            auto presenter = std::make_unique<gui::PowerOffPresenter>(app);
            return std::make_unique<gui::PowerOffWindow>(app, std::move(presenter));
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
        windowsFactory.attach(desktop_locked, [](Application *app, const std::string newname) {
            return std::make_unique<gui::LockedInfoWindow>(app);
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

        attachPopups({gui::popup::ID::Volume});
    }

    void ApplicationDesktop::destroyUserInterface()
    {}

    void ApplicationDesktop::activeSimChanged(std::string value)
    {
        auto sim = magic_enum::enum_cast<Store::GSM::SIM>(value);
        if (sim.has_value()) {
            Store::GSM::get()->selected = sim.value();
        }
        else {
            Store::GSM::get()->selected = Store::GSM::SIM::NONE;
        }

        if (Store::GSM::SIM::NONE == sim) {
            need_sim_select = true;
        }
    }

    void ApplicationDesktop::lockPassHashChanged(std::string value)
    {
        if (!value.empty()) {
            lockPassHash = utils::getNumericValue<unsigned int>(value);
        }
        else {
            lockPassHash = 0;
        }
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
        osCurrentVersion = std::move(value);
    }
    void ApplicationDesktop::setOsUpdateVersion(const std::string &value)
    {
        LOG_DEBUG("[ApplicationDesktop::setOsUpdateVersion] value=%s", value.c_str());
        if (value.empty()) {
            return;
        }
        osUpdateVersion = value;
        settings->setValue(
            settings::SystemProperties::osUpdateVersion, std::move(value), settings::SettingsScope::Global);
    }
} // namespace app
