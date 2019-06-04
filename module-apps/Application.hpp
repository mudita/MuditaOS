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
//module-sys
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "SystemManager/SystemManager.hpp"

namespace app {

/*
 * @brief This is template for creating new applications
 */
class Application: public sys::Service {
public:
	Application(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~Application();

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
	int switchWindow( const std::string& windowName, uint32_t cmd, uint32_t dataSize=0, uint8_t* data=nullptr );
	/**
	 * Generic function for returning to switch window in active application without performing initialization of the window.
	 */
	int switchBackWindow( const std::string& windowName, uint32_t cmd, uint32_t dataSize=0, uint8_t* data=nullptr );
	/**
	 * Method allows refreshing currently active window
	 */
	int refreshWindow(gui::RefreshModes mode);
	/**
	 * Sets active window of the application. This doesn't cause refresh. value -1 is for undefined state
	 * and should be set when application is left using back button.
	 */
	void setActiveWindow( const std::string& windowName );

//	settings_t* getSettings() {
//		return settings;
//	}

protected:
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

public:

};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_HPP_ */
