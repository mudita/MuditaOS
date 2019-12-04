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
//#include "gui/widgets/Window.hpp"
#include "gui/input/Translator.hpp"
//module-sys
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "SystemManager/SystemManager.hpp"
//module-db
#include "Interface/SettingsRecord.hpp"

#include "SwitchData.hpp"

// #define DEBUG_APPLICATION_MANAGEMENT

namespace gui {
	class AppWindow;
};

namespace app {

/*
 * @brief This is template for creating new applications
 */
class Application: public sys::Service {
    uint32_t longPressTimer = 0;
public:
	enum class State {
        /// app doesn't exist
        NONE,
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

    static const char *stateStr(State st);

private:

    State state = State::DEACTIVATED;

public:
	Application(std::string name, std::string parent="", bool startBackground = false, uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~Application();

    Application::State getState();
    void setState(State st);

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
	int switchWindow( const std::string& windowName, gui::ShowMode cmd = gui::ShowMode::GUI_SHOW_INIT, std::unique_ptr<gui::SwitchData> data = nullptr );
	/**
	 * Generic method for sending switch command. This will switch window within active application.
	 * It set the ShowMode to gui::ShowMode::GUI_SHOW_INIT
	 */
	inline auto switchWindow( const std::string& windowName, std::unique_ptr<gui::SwitchData> data ) {
		return switchWindow( windowName, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
	};
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

	virtual void setSuspendFlag( bool val ) { suspendInProgress = val; };
	virtual void setShutdownFlag() { shutdownInProgress = true; };

	//static methods
	static bool messageSwitchApplication( sys::Service* sender, std::string application, std::string window, std::unique_ptr<gui::SwitchData> data );
	static bool messageRefreshApplication( sys::Service* sender, std::string application, std::string window, gui::SwitchData* data=nullptr );
	static bool messageCloseApplication( sys::Service* sender, std::string application );
	static bool messageRebuildApplication( sys::Service* sender, std::string application );
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
	std::map<std::string, gui::AppWindow*> windows;
	gui::AppWindow* currentWindow = nullptr;
	gui::AppWindow* previousWindow = nullptr;
	/**
	 * Flag defines whether keyboard input is processed
	 */
	bool acceptInput = false;

	std::unique_ptr<gui::KeyInputSimpleTranslation> keyTranslator;
	/**
	 * Flag defines how application will behave after registration. It can go forground or background
	 */
	bool startBackground = false;
	/**
	 * Flag defines whether application initialized suspend mode, this will influence how render message will sent to gui service.
	 * If suspend is true, application manager will receive information from both eink and gui services if last rendering mesage will
	 * be processed.
	 */
	bool suspendInProgress = false;
	/**
	 * Flag defines case when display needs to be refreshed before closing the system. If flag is set to true next set of rendering commands will
	 * carry information to GUI service that system needs to be closed. After displaying the screen GUI will notify application manager to request system shutdown.
	 */
	bool shutdownInProgress = false;

	//protected static methods
	/**
	 *
	 */
	static bool messageInputEventApplication( sys::Service* sender, std::string application , const gui::InputEvent& event );
};

class ApplicationLauncher {
protected:
	//name of the application to run
	std::string name = "";
	//name of the application's owner
	std::string parent = "";
	//defines whether application can be closed when it looses focus
	bool closeable = true;
	//defines whether application should be run without gaining focus, it will remian in the BACKGROUND state
	bool startBackground = false;
	//flag defines whether this application can prevent power manager from changing
	bool preventBlocking = false;
public:
	ApplicationLauncher( std::string name, bool isCloseable, bool preventBlocking = false ) :
		name{name},
		closeable{isCloseable},
		preventBlocking{preventBlocking} {};
	virtual ~ApplicationLauncher() {};
	std::string getName() { return name;};
	bool isCloseable() { return closeable; };
	bool isBlocking() { return preventBlocking; };

	//virtual method to run the application
	virtual bool run(sys::Service* caller = nullptr ) {return true;};
	virtual bool runBackground(sys::Service* caller = nullptr ) {return true;};
    std::shared_ptr<Application> handle = nullptr;
};

template <class T>
class ApplicationLauncherT : public ApplicationLauncher
{
    public:
    ApplicationLauncherT(std::string name, bool isCloseable=true) : ApplicationLauncher(name, isCloseable) {}
    virtual bool run(sys::Service* caller) override {
        parent = (caller==nullptr?"":caller->GetName());
        handle = std::make_shared<T>(name, parent);
        return sys::SystemManager::CreateService( handle , caller );
	};
    bool runBackground(sys::Service *caller) override {
		parent = (caller==nullptr?"":caller->GetName());
        handle = std::make_shared<T>(name,parent, true);
    	return sys::SystemManager::CreateService( handle , caller );
    };
};

template <class T>
std::unique_ptr<ApplicationLauncherT<T>> CreateLauncher(std::string name, bool isCloseable=true) {
    return std::move(std::unique_ptr<ApplicationLauncherT<T>>(new ApplicationLauncherT<T>(name, isCloseable)));
}

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_HPP_ */
