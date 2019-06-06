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
#include "messages/APMMessage.hpp"

namespace sapm {

class ApplicationDescription {
public:

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
	app::Application::State state = app::Application::State::DEACTIVATED;
	//switching data stored when application manager had to run init function
	app::SwitchData* switchData;
};


/*
 *
 */
class ApplicationManager: public sys::Service {

	enum class State{
		IDLE,
		CLOSING_PREV_APP,
		WAITING_CLOSE_CONFIRMATION,
		STARTING_NEW_APP,
		WAITING_NEW_APP_CONFIRMATION,
		WAITING_LOST_FOCUS_CONFIRMATION,
		WAITING_GET_FOCUS_CONFIRMATION
	};

	std::map< std::string, ApplicationDescription* > applications;
	sys::SystemManager* systemManager;

	std::string currentApplicationName = "";
	std::string previousApplicationName = "";

	//tries to switch the application
	bool switchApplicationInternal( APMSwitch* msg);
	bool switchApplicationWithDataInternal( APMSwitchData* msg);
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

    /**
     * @brief Sends request to application manager to switch from current application to specific window in application with specified name .
     */
    static bool switchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName="" );
    /**
	 * @brief Sends request to application manager to switch from current application to specific window in application with specified name.
	 * Allows sending data to destination application.
	 */
    static bool switchApplicationWithData( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<app::SwitchData>& switchData );
    /**
     * @Brief Function allows sending confirmation to the switch request. This is sent both by application that gains and looses focus.
     */
	static bool confirmSwitch( sys::Service* sender);
	/**
	 * @brief Function allows application to confirm close request.
	 */
    static bool confirmClose( sys::Service* sender);
    /**
     * @brief Allows requesting Application Manager to run previous application.
     */
    static bool switchPreviousApplication( sys::Service* sender, const std::string& prevAppName );
};

} /* namespace sapm */

#endif /* MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_ */
