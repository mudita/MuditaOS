#include "ApplicationDesktop.hpp"

#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/PinLockWindow.hpp"
#include "windows/PowerOffWindow.hpp"
#include "windows/Reboot.hpp"

#include <service-db/api/DBServiceAPI.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <messages/QueryMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>

#include <cassert>
namespace app
{

    ApplicationDesktop::ApplicationDesktop(std::string name, std::string parent, bool startBackground)
        : Application(name, parent)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    ApplicationDesktop::~ApplicationDesktop()
    {
        LOG_INFO("Desktop destruktor");
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationDesktop::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        bool handled = false;
        if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<cellular::StateChange *>(msgl)) {
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

    auto ApplicationDesktop::handle(db::query::notifications::GetAllResult *msg) -> bool
    {
        assert(msg);
        auto records = *msg->getResult();
        for (auto record : records) {
            switch (record.key) {
            case NotificationsRecord::Key::Calls:
                notifications.notSeen.Calls = record.value;
                break;

            case NotificationsRecord::Key::Sms:
                notifications.notSeen.SMS = record.value;
                break;

            case NotificationsRecord::Key::NotValidKey:
            case NotificationsRecord::Key::NumberOfKeys:
                LOG_ERROR("Not a valid key");
                return false;
            }
        }

        windows[app::window::name::desktop_menu]->rebuild();
        windows[app::window::name::desktop_main_window]->rebuild();
        return true;
    }

    auto ApplicationDesktop::handle(db::NotificationMessage *msg) -> bool
    {
        assert(msg);
        if (msg->interface == db::Interface::Name::Settings) {
            reloadSettings();
        }

        if (msg->interface == db::Interface::Name::Notifications && msg->type == db::Query::Type::Update) {
            return requestNotSeenNotifications();
        }

        if ((msg->interface == db::Interface::Name::Calllog || msg->interface == db::Interface::Name::SMSThread ||
             msg->interface == db::Interface::Name::SMS) &&
            msg->type != db::Query::Type::Read) {
            requestNotReadNotifications();
            windows[app::window::name::desktop_menu]->rebuild();
        }

        return false;
    }

    auto ApplicationDesktop::handle(cellular::StateChange *msg) -> bool
    {
        assert(msg);
        if (msg->request == cellular::State::ST::URCReady) {
            if (need_sim_select && !screenLocked) {
                sapm::ApplicationManager::messageSwitchApplication(this, app::name_settings, app::sim_select, nullptr);
                return true;
            }
            else if (need_sim_select == false) {
                sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularSimProcedure),
                                      ServiceCellular::serviceName,
                                      this);
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
        return sapm::ApplicationManager::messageSwitchApplication(
            this, app::CallLogAppStr, gui::name::window::main_window, nullptr);
    }

    bool ApplicationDesktop::clearCallsNotification()
    {
        LOG_DEBUG("Clear Call notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
        notifications.notSeen.Calls = 0;
        getCurrentWindow()->rebuild(); // triger rebuild - shouldn't be needed, but is
        return true;
    }

    bool ApplicationDesktop::clearMessagesNotification()
    {
        LOG_DEBUG("Clear Sms notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
        notifications.notSeen.SMS = 0;
        getCurrentWindow()->rebuild(); // triger rebuild - shouldn't be needed, but is
        return true;
    }

    bool ApplicationDesktop::requestNotSeenNotifications()
    {
        return DBServiceAPI::GetQuery(
            this, db::Interface::Name::Notifications, std::make_unique<db::query::notifications::GetAll>());
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

        screenLocked = true;

        reloadSettings();
        requestNotReadNotifications();
        requestNotSeenNotifications();

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationDesktop::DeinitHandler()
    {
        LOG_INFO("DeinitHandler");
        return sys::ReturnCodes::Success;
    }

    void ApplicationDesktop::createUserInterface()
    {
        using namespace app::window::name;
        windows.insert(std::pair<std::string, gui::AppWindow *>(desktop_main_window, new gui::DesktopMainWindow(this)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(desktop_pin_lock, new gui::PinLockWindow(this)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(desktop_menu, new gui::MenuWindow(this)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(desktop_poweroff, new gui::PowerOffWindow(this)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(desktop_reboot, new gui::RebootWindow(this)));
    }

    bool ApplicationDesktop::getScreenLocked()
    {
        return screenLocked;
    }

    bool ApplicationDesktop::getPinLocked()
    {
        return pinLocked;
    }

    void ApplicationDesktop::setScreenLocked(bool val)
    {
        screenLocked = val;
    };

    void ApplicationDesktop::destroyUserInterface()
    {}

    void ApplicationDesktop::reloadSettings()
    {
        settings = DBServiceAPI::SettingsGet(this);

        pinLocked = settings.lockPassHash != 0;

        switch (settings.activeSIM) {
        case SettingsRecord::ActiveSim::NONE:
            Store::GSM::get()->selected = Store::GSM::SIM::NONE;
            need_sim_select             = true;
            break;
        case SettingsRecord::ActiveSim::SIM1:
            Store::GSM::get()->selected = Store::GSM::SIM::SIM1;
            break;
        case SettingsRecord::ActiveSim::SIM2:
            Store::GSM::get()->selected = Store::GSM::SIM::SIM2;
            break;
        }
    }
} // namespace app
