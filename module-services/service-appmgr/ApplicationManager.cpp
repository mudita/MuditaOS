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
#include <utility>
#include <memory>

namespace sapm {

ApplicationDescription::ApplicationDescription( std::string name, std::unique_ptr<app::ApplicationLauncher> lanucher, bool closeable ) :
	name{name}, closeable{closeable}, state{ApplicationState::DEACTIVATED}, switchData{nullptr} {
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
	return std::make_shared<sys::ResponseMessage>();
}
// Invoked when timer ticked
void ApplicationManager::TickHandler(uint32_t id) {

}

// Invoked during initialization
sys::ReturnCodes ApplicationManager::InitHandler() {

	//search for application with specified name and run it
	//TODO name of the app should be Homescreen but for tests is is "ApplicationClock"
	std::string runAppName = "ApplicationClock";

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

} /* namespace sapm */
