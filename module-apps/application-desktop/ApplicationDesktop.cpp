// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "Dialog.hpp"
#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "locks/windows/PinLockWindow.hpp"
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
#include <windows/Dialog.hpp>
#include <windows/DialogMetadata.hpp>
#include <messages/DialogMetadataMessage.hpp>

#include "AppWindow.hpp"
#include "locks/data/LockData.hpp"
#include "data/DesktopData.hpp"
#include "models/ActiveNotificationsModel.hpp"

#include <service-db/DBServiceAPI.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <service-db/QueryMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <module-db/queries/messages/threads/QueryThreadsGetCount.hpp>
#include <module-db/queries/calllog/QueryCalllogGetCount.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/magic_enum/include/magic_enum.hpp>
#include <module-apps/messages/AppMessage.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <service-db/DBCalllogMessage.hpp>
#include <module-gui/gui/widgets/TopBar/SIM.hpp>

#include <cassert>
namespace app
{

    namespace
    {

        bool requestUnreadThreadsCount(app::Application *app)
        {
            const auto [succeed, _] = DBServiceAPI::GetQuery(
                app, db::Interface::Name::SMSThread, std::make_unique<db::query::ThreadGetCount>(EntryState::UNREAD));
            return succeed;
        }

        bool requestUnreadCallsCount(app::Application *app)
        {
            const auto [succeed, _] = DBServiceAPI::GetQuery(
                app, db::Interface::Name::Calllog, std::make_unique<db::query::CalllogGetCount>(EntryState::UNREAD));
            return succeed;
        }
    } // namespace

    ApplicationDesktop::ApplicationDesktop(std::string name,
                                           std::string parent,
                                           sys::phone_modes::PhoneMode mode,
                                           StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground),
          lockHandler(this), notificationsModel{std::make_shared<gui::ActiveNotificationsModel>()}
    {
        using namespace gui::top_bar;
        topBarManager->enableIndicators({Indicator::Signal,
                                         Indicator::Time,
                                         Indicator::Battery,
                                         Indicator::SimCard,
                                         Indicator::NetworkAccessTechnology});
        topBarManager->set(Indicator::SimCard,
                           std::make_shared<SIMConfiguration>(SIMConfiguration::DisplayMode::OnlyInactiveState));
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

        addActionReceiver(app::manager::actions::RequestPin, [this](auto &&data) {
            lockHandler.handlePasscodeRequest(locks::Lock::LockType::SimPin, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestScreenPasscode, [this](auto &&data) {
            lockHandler.handleScreenPasscodeRequest(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPuk, [this](auto &&data) {
            lockHandler.handlePasscodeRequest(locks::Lock::LockType::SimPuk, std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPinChange, [this](auto &&data) {
            lockHandler.handlePinChangeRequest(std::move(data));
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPinDisable, [this](auto &&data) {
            lockHandler.handlePinEnableRequest(std::forward<decltype(data)>(data),
                                               CellularSimCardLockDataMessage::SimCardLock::Unlocked);
            return actionHandled();
        });

        addActionReceiver(app::manager::actions::RequestPinEnable, [this](auto &&data) {
            lockHandler.handlePinEnableRequest(std::forward<decltype(data)>(data),
                                               CellularSimCardLockDataMessage::SimCardLock::Locked);
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
                if (auto response = dynamic_cast<db::query::ThreadGetCountResult *>(result.get())) {
                    handled = handle(response);
                }
                if (auto response = dynamic_cast<db::query::CalllogGetCountResult *>(result.get())) {
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

    void ApplicationDesktop::handle(manager::actions::NotificationsChangedParams *params)
    {
        if (getCurrentWindow()->getName() == app::window::name::desktop_main_window) {
            notificationsModel->updateData(params);
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
    }

    auto ApplicationDesktop::handle(db::NotificationMessage *msg) -> bool
    {
        assert(msg);
        if (msg->interface == db::Interface::Name::Calllog && msg->type != db::Query::Type::Read) {
            return requestUnreadCallsCount(this);
        }

        if ((msg->interface == db::Interface::Name::SMSThread || msg->interface == db::Interface::Name::SMS) &&
            msg->type != db::Query::Type::Read) {
            return requestUnreadThreadsCount(this);
        }

        return false;
    }

    auto ApplicationDesktop::handle(db::query::ThreadGetCountResult *msg) -> bool
    {
        if (msg->getState() == EntryState::UNREAD) {
            notifications.notRead.SMS = msg->getCount();
        }
        return refreshMenuWindow();
    }

    auto ApplicationDesktop::handle(db::query::CalllogGetCountResult *msg) -> bool
    {
        if (msg->getState() == EntryState::UNREAD) {
            notifications.notRead.Calls = msg->getCount();
        }
        return refreshMenuWindow();
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

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        lockPassHashChanged(
            settings->getValue(settings::SystemProperties::lockPassHash, settings::SettingsScope::Global));

        requestUnreadThreadsCount(this);
        requestUnreadCallsCount(this);
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

        auto createPinChangedSuccessfullyDialog =
            [](app::ApplicationDesktop *app) -> std::unique_ptr<gui::DialogMetadataMessage> {
            return std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::translate("app_desktop_sim_change_pin"),
                                    "success_icon_W_G",
                                    utils::translate("app_desktop_sim_pin_changed_successfully"),
                                    "",
                                    [app]() {
                                        app->switchWindow(app::window::name::desktop_main_window);
                                        return true;
                                    }});
        };

        connect(typeid(CellularSimNewPinResponseMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto response = dynamic_cast<CellularSimNewPinResponseMessage *>(request);
            if (response->retCode) {
                auto metaData = createPinChangedSuccessfullyDialog(this);
                switchWindow(gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            }
            else {
                lockHandler.handlePinChangeRequestFailed();
            }
            return sys::MessageNone{};
        });

        connect(typeid(CellularSimPukResponseMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto response = dynamic_cast<CellularSimPukResponseMessage *>(request);
            if (response->retCode) {
                auto metaData = createPinChangedSuccessfullyDialog(this);
                switchWindow(gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            }
            return sys::MessageNone{};
        });

        connect(typeid(CellularSimCardLockResponseMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto response = dynamic_cast<CellularSimCardLockResponseMessage *>(request);
            if (response->retCode) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                    "",
                    "success_icon_W_G",
                    response->getSimCardLock() == CellularSimCardLockDataMessage::SimCardLock::Unlocked
                        ? utils::translate("app_desktop_sim_card_unlocked")
                        : utils::translate("app_desktop_sim_card_locked"),
                    "",
                    [this]() {
                        switchWindow(app::window::name::desktop_main_window);
                        return true;
                    }});
                switchWindow(gui::window::name::dialog_confirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            }
            else {
                lockHandler.handlePinEnableRequestFailed(response->getSimCardLock());
            }
            return sys::MessageNone{};
        });

        auto msgToSend =
            std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateCheckForUpdateOnce);
        bus.sendUnicast(msgToSend, service::name::service_desktop);

        auto selectedSim = magic_enum::enum_cast<Store::GSM::SIM>(
            settings->getValue(settings::SystemProperties::activeSim, settings::SettingsScope::Global));
        if (selectedSim.has_value()) {
            Store::GSM::get()->selected = selectedSim.value();
        }
        else {
            Store::GSM::get()->selected = Store::GSM::SIM::NONE;
        }

        settings->registerValueChange(
            settings::SystemProperties::activeSim,
            [this](const std::string &value) { activeSimChanged(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::lockPassHash,
            [this](const std::string &value) { lockPassHashChanged(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::osCurrentVersion,
            [this](const std::string &value) { osCurrentVersionChanged(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::osUpdateVersion,
            [this](const std::string &value) { osUpdateVersionChanged(value); },
            settings::SettingsScope::Global);

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
        windowsFactory.attach(desktop_main_window, [this](Application *app, const std::string &name) {
            return std::make_unique<gui::DesktopMainWindow>(app, notificationsModel);
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
        windowsFactory.attach(gui::window::name::dialog_confirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes});
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
