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
#include "service-db/messages/DBNotificationMessage.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/PinLockWindow.hpp"
#include "windows/PowerOffWindow.hpp"

#include "ApplicationDesktop.hpp"
#include "service-db/api/DBServiceAPI.hpp"

namespace app {

    ApplicationDesktop::ApplicationDesktop(std::string name, std::string parent, bool startBackground) : Application(name, parent)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

ApplicationDesktop::~ApplicationDesktop() {
	LOG_INFO("Desktop destruktor");
}

// Invoked upon receiving data message
sys::Message_t ApplicationDesktop::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

    if (msgl->messageType == MessageType::DBServiceNotification)
    {
        auto *msg = dynamic_cast<DBNotificationMessage *>(msgl);
        LOG_DEBUG("Received multicast");
        if ((msg != nullptr) && ((msg->notificationType == DB::NotificationType::Updated) || (msg->notificationType == DB::NotificationType::Added)))
        {
            notifications.notSeenCalls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
            notifications.notSeenSMS = DBServiceAPI::SMSGetCount(this, EntryState::UNREAD);
            this->windows[app::name::window::desktop_menu]->rebuild();
            this->windows[app::name::window::desktop_lockscreen]->rebuild();
            return std::make_shared<sys::ResponseMessage>();
        }
    }

    bool handled = true;

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationDesktop::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	//if value of the pin hash is different than 0 it means that home screen is pin protected
	if( settings.lockPassHash ) {
		pinLocked = true;
	}

	screenLocked = true;

    notifications.notSeenCalls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
    notifications.notSeenSMS = DBServiceAPI::SMSGetCount(this, EntryState::UNREAD);

    createUserInterface();

    setActiveWindow(gui::name::window::main_window);

    return ret;
}

sys::ReturnCodes ApplicationDesktop::DeinitHandler() {
	LOG_INFO("DeinitHandler");
	return sys::ReturnCodes::Success;
}

void ApplicationDesktop::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::DesktopMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::PinLockWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>( window->getName(), window));

	window = new gui::MenuWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>( window->getName(), window));

	window = new gui::PowerOffWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>( window->getName(), window));
}

bool ApplicationDesktop::getScreenLocked() {
	return screenLocked;
}

bool ApplicationDesktop::getPinLocked() {
	return pinLocked;
}

void ApplicationDesktop::setScreenLocked( bool val ) {
	screenLocked = val;
};

void ApplicationDesktop::destroyUserInterface() {
}

} /* namespace gui */
