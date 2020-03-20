/*
 * @file ApplicationDesktop.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 18 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/PinLockWindow.hpp"
#include "windows/PowerOffWindow.hpp"

#include "ApplicationDesktop.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <application-settings/ApplicationSettings.hpp>
#include <cassert>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include "windows/Reboot.hpp"

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
        if (auto msg = dynamic_cast<DBNotificationMessage *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<cellular::StateChange *>(msgl)) {
            handled = handle(msg);
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    auto ApplicationDesktop::handle(DBNotificationMessage *msg) -> bool
    {
        LOG_DEBUG("Received multicast");
        assert(msg);
        if ((msg->notificationType == DB::NotificationType::Updated) ||
            (msg->notificationType == DB::NotificationType::Added)) {
            notifications.notSeenCalls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
            notifications.notSeenSMS   = DBServiceAPI::SMSGetCount(this, EntryState::UNREAD);
            windows[app::window::name::desktop_menu]->rebuild();
            windows[app::window::name::desktop_main_window]->rebuild();
            return true;
        }
        return false;
    }

    auto ApplicationDesktop::handle(cellular::StateChange *msg) -> bool
    {
        assert(msg);
        if (msg->request == cellular::State::ST::ModemOn) {
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

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        // if value of the pin hash is different than 0 it means that home screen is pin protected
        if (settings.lockPassHash != 0) {
            pinLocked = true;
        }

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

        screenLocked = true;

        notifications.notSeenCalls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
        notifications.notSeenSMS   = DBServiceAPI::SMSGetCount(this, EntryState::UNREAD);

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
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

} // namespace app
