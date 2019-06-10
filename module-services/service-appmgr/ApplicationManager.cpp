/*
 * @file ApplicationManager.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "SystemManager/SystemManager.hpp"
#include "ApplicationManager.hpp"
#include "messages/APMMessage.hpp"
#include <utility>
#include <memory>

//module-utils
#include "log/log.hpp"

namespace sapm {

ApplicationDescription::ApplicationDescription( std::string name, std::unique_ptr<app::ApplicationLauncher> lanucher, bool closeable ) :
	name{name}, closeable{closeable}, state{app::Application::State::DEACTIVATED}, switchData{nullptr} {
	this->lanucher = std::move(lanucher);
}

ApplicationManager::ApplicationManager( const std::string& name, sys::SystemManager* sysmgr,
		std::vector< std::unique_ptr<app::ApplicationLauncher> >& launchers ) : Service(name), systemManager{sysmgr} {
	//store the pointers in the map where key is the name of the app and value is the launcher
	for( uint32_t i=0; i<launchers.size(); ++i ) {

		std::string name = launchers[i]->getName();
		bool isCloseable = launchers[i]->isCloseable();
		ApplicationDescription* desc = new ApplicationDescription(name, std::move(launchers[i]), isCloseable );

		applications.insert(std::pair<std::string, ApplicationDescription*>(name, desc)	);
	}
}
ApplicationManager::~ApplicationManager() {
	for( auto it = applications.begin(); it!=applications.end(); it++ ) {
		delete it->second;
	}

}

sys::Message_t ApplicationManager::DataReceivedHandler(sys::DataMessage* msgl) {

	uint32_t msgType = msgl->messageType;

	switch( msgType ) {
		case static_cast<uint32_t>(MessageType::APMSwitch): {
			sapm::APMSwitch* msg = reinterpret_cast<sapm::APMSwitch*>( msgl );
			LOG_INFO("APMSwitch %s", msg->getName());
		}break;
		case static_cast<uint32_t>(MessageType::APMSwitchData): {
			sapm::APMSwitchData* msg = reinterpret_cast<sapm::APMSwitchData*>( msgl );
			LOG_INFO("APMSwitchData %s", msg->getName());
		}break;
		case static_cast<uint32_t>(MessageType::APMSwitchPrevApp): {
			sapm::APMSwitchPrevApp* msg = reinterpret_cast<sapm::APMSwitchPrevApp*>( msgl );
			LOG_INFO("APMSwitchPrevApp %s", msg->getName());
		}break;
		case static_cast<uint32_t>(MessageType::APMConfirmSwitch): {
			sapm::APMConfirmSwitch* msg = reinterpret_cast<sapm::APMConfirmSwitch*>( msgl );
			LOG_INFO("APMConfirmSwitch %s", msg->getName());
		}break;
		case static_cast<uint32_t>(MessageType::APMConfirmClose): {
			sapm::APMConfirmClose* msg = reinterpret_cast<sapm::APMConfirmClose*>( msgl );
			LOG_INFO("APMConfirmClose %s", msg->getName());
		}break;
		default : {
			LOG_FATAL("Received unknown massage %d", msgType );
		} break;
	};


	return std::make_shared<sys::ResponseMessage>();
}
// Invoked when timer ticked
void ApplicationManager::TickHandler(uint32_t id) {

}

// Invoked during initialization
sys::ReturnCodes ApplicationManager::InitHandler() {

	//search for application with specified name and run it
	//TODO name of the app should be Homescreen but for tests is is "ApplicationClock"
	std::string runAppName = "ApplicationViewer";

	auto it = applications.find(runAppName);
	if( it!= applications.end()){
		it->second->lanucher->run(systemManager);
	}

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::SleepHandler() {
	return sys::ReturnCodes::Success;
}

bool ApplicationManager::switchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName ) {

	auto msg = std::make_shared<sapm::APMSwitch>(applicationName, windowName );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}
bool ApplicationManager::switchApplicationWithData( sys::Service* sender,
		const std::string& applicationName,
		const std::string& windowName,
		std::unique_ptr<app::SwitchData>& switchData ) {
			auto msg = std::make_shared<sapm::APMSwitchData>(applicationName, windowName, std::move(switchData) );
			sys::Bus::SendUnicast(msg, "ApplicationManager", sender);

	return true;
}
bool ApplicationManager::confirmSwitch( sys::Service* sender) {
	return true;
}
bool ApplicationManager::confirmClose( sys::Service* sender) {
	return true;
}
bool ApplicationManager::switchPreviousApplication( sys::Service* sender, const std::string& prevAppName ) {

	return true;
}

} /* namespace sapm */
