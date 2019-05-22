/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "gui/core/Font.hpp"
#include "gui/core/PixMapManager.hpp"

#include "ServiceGUI.hpp"
#include "log/log.hpp"

ServiceGUI::ServiceGUI(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[ServiceGUI] Initializing");

	gui::FontManager& fontManager = gui::FontManager::getInstance();
	fontManager.init( "sys/assets" );

	gui::PixMapManager& pixMapManager = gui::PixMapManager::getInstance();
	pixMapManager.init( "sys/assets" );
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("[ServiceGUI] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceGUI::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceGUI::SleepHandler() {
	return sys::ReturnCodes::Success;
}




