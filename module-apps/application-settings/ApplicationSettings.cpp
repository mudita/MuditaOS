/*
 * @file ApplicationSesttings.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/SettingsMainWindow.hpp"
#include "windows/LanguageWindow.hpp"
#include "windows/BtWindow.hpp"

#include "windows/UITestWindow.hpp"

#include "windows/TestMessageWindow.hpp"


#include "ApplicationSettings.hpp"

#include "service-cellular/ServiceCellular.hpp"

namespace app {

ApplicationSettings::ApplicationSettings(std::string name, std::string parent, bool startBackgound) :
	Application( name, parent, startBackgound ) {
	busChannels.push_back(sys::BusChannels::ServiceCellularSMSNotification);
}

ApplicationSettings::~ApplicationSettings() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationSettings::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	//this variable defines whether message was processed.
	bool handled = true;

//	if( msgl->messageType == static_cast<int32_t>(MessageType::CellularSMSMulticast) )
//	{
//		CellularSMSRequestMessage *msg = reinterpret_cast<CellularSMSRequestMessage*>(msgl);
//		auto pos = windows.find("Message");
//		if(pos != windows.end())
//		{
//			if( currentWindow == pos->second)
//			{
//				gui::TestMessageWindow* window = reinterpret_cast<gui::TestMessageWindow*>(pos->second);
//				window->cellularMessageCallback(msg->number, msg->message);
//			}
//		}
//	}


	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationSettings::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	createUserInterface();

	setActiveWindow("MainWindow");

	return ret;
}

sys::ReturnCodes ApplicationSettings::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationSettings::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::SettingsMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::LanguageWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::BtWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::UiTestWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

//	window = new gui::TestMessageWindow(this);
//	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));
}

void ApplicationSettings::destroyUserInterface() {
}

} /* namespace app */
