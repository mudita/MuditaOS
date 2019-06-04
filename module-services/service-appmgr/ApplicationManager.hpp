/*
 * @file ApplicationManager.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_
#define MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <string.h>
#include "Application.hpp"
#include "SystemManager/SystemManager.hpp"

namespace sapm {

//class strores information that was sent along with switch message
class SwitchData {
public:

	SwitchData( uint8_t* data, uint32_t size ) : data{nullptr}, size{0} {
		this->data = new uint8_t[size];
		if( data ) {
			memcpy( this->data, data, size );
			this->size = size;
		}
		else
			this->size = 0;
	}
	~SwitchData() {
		if( data )
			delete []data;
	}

	uint8_t* data;
	uint32_t size;
};

class ApplicationDescription {
public:
	enum class ApplicationState {
		//Application is registered. Name of the application has been provided along with a function to run.
		//Such application can be activated using provided init function.
		DEACTIVATED,
		//Initialization is triggered by the switch message sent by other application to the application manager.
		//Upon receiving such message application manager is saving switch message to send it later to the application.
		//Init function of the application has been activated. Application should create/register all needed channels.
		//When all channels are ready application will try to register in the application manager by sending APP_REGISTER message
		INITIALIZING,
		//Application manager sent variant of switch command to the selected application and it's now waiting for confirmation
		//from the application
		ACTIVATING,
		//Application is working and has focus and can render
		ACTIVE_FORGROUND,
		//Applicatino lost focus but it is still working
		ACTIVE_BACKGROUND,
		//Application Manager sent signal to application that it should be closed. Application mus perform all necessary cleanup
		//and request System Manager to close it.
		DEACTIVATING
	};

	ApplicationDescription( std::string name, std::unique_ptr<app::ApplicationLauncher> lanucher, bool closeable );
	~ApplicationDescription() {
		if( switchData )
			delete switchData;
	}
	//name of the application. It's used to find proper application during switching
	std::string name;
	//launcher to application to the application's start function
	std::unique_ptr<app::ApplicationLauncher> lanucher;
	//informs if it is possible to close application when it looses focus.
	bool closeable;
	//current state of the application
	ApplicationState state;
	//switching data stored when application manager had to run init function
	SwitchData* switchData;
};


/*
 *
 */
class ApplicationManager: public sys::Service {
	std::map< std::string, ApplicationDescription* > applications;
	sys::SystemManager* systemManager;
public:
	ApplicationManager( const std::string& name, sys::SystemManager* sysmgr, std::vector< std::unique_ptr<app::ApplicationLauncher> >& launchers );
    ~ApplicationManager();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;
};

} /* namespace sapm */

#endif /* MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_ */
