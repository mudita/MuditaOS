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
#include "i18/i18.hpp"

namespace sapm {

class ApplicationDescription {
public:
	ApplicationDescription( std::string name, std::unique_ptr<app::ApplicationLauncher> lanucher, bool closeable );
	virtual ~ApplicationDescription() {}
	//name of the application. It's used to find proper application during switching
	std::string name;
	//launcher to application to the application's start function
	std::unique_ptr<app::ApplicationLauncher> lanucher;
	//informs if it is possible to close application when it looses focus.
	bool closeable;
	//current state of the application
	app::Application::State state = app::Application::State::DEACTIVATED;
	//switching data stored when application manager had to run init function
	std::unique_ptr<gui::SwitchData> switchData = nullptr;
	std::string switchWindow = "";
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
		WAITING_NEW_APP_REGISTRATION,
		WAITING_LOST_FOCUS_CONFIRMATION,
		WAITING_GET_FOCUS_CONFIRMATION
	};

	SettingsRecord settings;

	std::map< std::string, ApplicationDescription* > applications;
	sys::SystemManager* systemManager;

	//
	std::unique_ptr<utils::i18> i18;

	//application that currently has focus. This means that is has rights to display screens and receive keyboard events.
	std::string focusApplicationName = "";
	//after loosing focus application becomes previous application and this is its name
	std::string previousApplicationName = "";
	//name of the application scheduled for launching
	std::string launchApplicationName = "";
	//state of the application manager
	State state = State::IDLE;

	//tries to switch the application
	bool handleSwitchApplication( APMSwitch* msg);
//	bool handleSwitchApplicationWithData( APMSwitchWithData* msg);
	bool handleCloseConfirmation( APMConfirmClose* msg );
	bool handleSwitchConfirmation( APMConfirmSwitch* msg );
	bool handleSwitchPrevApplication( APMSwitchPrevApp* msg );
	bool handleRegisterApplication( APMRegister* msg );
	bool handleLanguageChange( sapm::APMChangeLanguage* msg );
	bool startApplication( const std::string& appName );
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
    static bool messageSwitchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<gui::SwitchData> data );
    /**
	 * @brief Sends request to application manager to switch from current application to specific window in application with specified name.
	 * Allows sending data to destination application.
	 */
//    static bool messageSwitchApplicationWithData( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<app::SwitchData>& switchData );
    /**
     * @Brief Function allows sending confirmation to the switch request. This is sent both by application that gains and looses focus.
     */
	static bool messageConfirmSwitch( sys::Service* sender);
	/**
	 * @brief Function allows application to confirm close request.
	 */
    static bool messageConfirmClose( sys::Service* sender);
    /**
     * @brief Allows requesting Application Manager to run previous application.
     */
    static bool messageSwitchPreviousApplication( sys::Service* sender );
    /**
	* @brief Sends information from application to manager about result of application's init function.
	* If successful message will contain name and true value, otherwise false value will be transmitted.
	*/
   static bool messageRegisterApplication( sys::Service* sender, const bool& status );
   /**
    * @brief Sends message to application manager to inform it about change of the phone's language performed by the user.
    */
   static bool messageChangeLanguage( sys::Service* sender, utils::Lang language );
};

} /* namespace sapm */

#endif /* MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_ */
