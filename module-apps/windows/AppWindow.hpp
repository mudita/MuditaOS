/*
 * @file AppWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_WINDOWS_APPWINDOW_HPP_
#define MODULE_APPS_WINDOWS_APPWINDOW_HPP_

#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Window.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

namespace app {
	class Application;
};


namespace gui {

/*
 * @brief This is wrapper for gui window used within applications.
 */
class AppWindow: public Window {
protected:
	/**
	 * Information bar for the buttons on the bottom of the page.
	 */
	gui::BottomBar* bottomBar = nullptr;
	/**
	 * Information bar for signal, battery and lock icon on the top of the screen.
	 */
	gui::TopBar* topBar = nullptr;
	/**
	 * Pointer to the application object that owns the window.
	 */
	app::Application* application = nullptr;
	uint32_t calculateBatteryLavel( uint32_t percentage );
public:

	AppWindow( app::Application* app, std::string name, uint32_t id=GUIWindowID++ );
	AppWindow( AppWindow* win );
	virtual ~AppWindow();

	app::Application* getApplication() { return application; };
	void setApplication( app::Application* app ) { application = app; };
	virtual bool onDatabaseMessage( sys::Message* msg );

	//updates battery level in the window
	bool updateBatteryLevel( uint32_t percentage );
	//updates battery level in the window
	bool updateSignalStrength( uint32_t strength );

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
	std::list<DrawCommand*> buildDrawList() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_WINDOWS_APPWINDOW_HPP_ */
