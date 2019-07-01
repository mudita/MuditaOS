/*
 * @file Application.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_HPP_
#define MODULE_APPS_APPLICATION_HPP_

#include <map>
//module-gui
#include "gui/Common.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/input/Translator.hpp"
//module-sys
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "SystemManager/SystemManager.hpp"
//module-db
#include "Interface/SettingsRecord.hpp"

#include "SwitchData.hpp"

namespace app {

/*
 * @brief This is template for creating new applications
 */
class Application: public sys::Service {
public:
	enum class State {
		//Application: Object has been created and underlying service is waiting to execute init handler method.
		//Application Manager: Launcher for the application has been provided. Application can be started using provided launcher. The other possibility
		//is that Appication Manager received CLOSING_FINISHED message.
		DEACTIVATED,
		//Application: Set after entering the init handler of the application. In this state application will request
		//in a blocking way the db configuration of the phone. Before exiting the init handler application must send APP_REGISTER message to
		//Application Manager.
		//Application Manager: Initialization is triggered by the switch message sent by other application to the application manager.
		//Launcher for the application has been executed upon receiving switch command. Optional switch data has been
		//saved and it will be provided when application manager receives registration message.
		INITIALIZING,
		//Application manager sent variant of switch command to the selected application and it's now waiting for confirmation
		//from the application
		ACTIVATING,
		//Application is working and has focus and can render
		ACTIVE_FORGROUND,
		//Applicatino lost focus but it is still working
		ACTIVE_BACKGROUND,
		//Application: Close request message has been received. As a response application must send close request acknowledge message.
		//Application must start closing all workers and releasing resources. After all workers are closed and resources released application
		//sends to application manager CLOSING_FINISHED message
		//Application Manager sent signal to application that it should be closed. Application must perform all necessary cleanup
		//and request System Manager to close it.
		DEACTIVATING
	};
public:
	Application(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~Application();

	/**
	 * Virtual methods
	 */
	void TickHandler(uint32_t id) override;

	/**
	 * Method responsible for rendering currently active window.
	 */
	void render( gui::RefreshModes mode );
	/**
	 * Method responsible for setting application to the state where incoming user input is blocked
	 */
	void blockEvents(bool isBlocked );
	/**
	 * Generic function for sending switch command. This will switch window within active application.
	 */
	int switchWindow( const std::string& windowName, uint32_t cmd, std::unique_ptr<gui::SwitchData> data );
	/**
	 * Generic function for returning to switch window in active application without performing initialization of the window.
	 */
	int switchBackWindow( const std::string& windowName, uint32_t cmd, std::unique_ptr<gui::SwitchData> data );
	/**
	 * Method allows refreshing currently active window
	 */
	int refreshWindow(gui::RefreshModes mode);

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl);
	/**
	 * Sets active window of the application. This doesn't cause refresh. value -1 is for undefined state
	 * and should be set when application is left using back button.
	 */
	sys::ReturnCodes InitHandler() override;
	void setActiveWindow( const std::string& windowName );

	void setKeyboardProfile( const std::string& profile );

	SettingsRecord& getSettings() {
		return settings;
	}

	//static methods
	static bool messageSwitchApplication( sys::Service* sender, std::string application, std::string window, std::unique_ptr<gui::SwitchData> data );
	static bool messageRefreshApplication( sys::Service* sender, std::string application, std::string window, gui::SwitchData* data=nullptr );
	static bool messageCloseApplication( sys::Service* sender, std::string application );
	/**
	 * @brief This method is used to send message to set focus of the application.
	 * Application can gain or lose focus depending on the provided focus flag.
	 */
//	static bool messageFocusApplication( sys::Service* sender, std::string application, bool focus );

protected:
	//application's settings taken from database
	SettingsRecord settings;
	/**
	 * Placeholder that can be used to create window and widgets.
	 */
	virtual void createUserInterface() = 0;
	/**
	 * Placeholder for closing application's windows.
	 */
	virtual void destroyUserInterface() = 0;
	/**
	 * Map containing application's windows
	 */
	std::map<std::string, gui::Window*> windows;
	gui::Window* currentWindow = nullptr;
	gui::Window* previousWindow = nullptr;
	/**
	 * Flag defines whether keyboard input is processed
	 */
	bool acceptInput = false;
	/**
	 * State of the application
	 */
	State state = State::DEACTIVATED;
	/**
	 * Timer for checking long press timeouts
	 */
	uint32_t longpressTimerID = 0;
	/**
	 * Object responsible for translating keyboard events into gui acceptable events
	 */
	std::unique_ptr<gui::Translator> translator;

	//protected static methods
	/**
	 *
	 */
	static bool messageInputEventApplication( sys::Service* sender, std::string application , const gui::InputEvent& event );
};

class ApplicationLauncher {
protected:
	//name of the application to run
	std::string name;
	//defines whether application can be closed when it looses focus
	bool closeable = true;
public:
	ApplicationLauncher( std::string name, bool isCloseable ) : name{name}, closeable{isCloseable} {};
	virtual ~ApplicationLauncher() {};
	std::string getName() { return name;};
	bool isCloseable() { return closeable; };
	//virtual method to run the application
	virtual bool run(sys::SystemManager* sysmgr) {return true;};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_HPP_ */
