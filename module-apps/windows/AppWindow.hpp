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
#include "Application.hpp"


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
public:

	AppWindow( app::Application* app, std::string name, uint32_t id=GUIWindowID++ );
	AppWindow( AppWindow* win );
	virtual ~AppWindow();

	app::Application* getApplication() { return application; };
	void setApplication( app::Application* app ) { application = app; };

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;

};

} /* namespace gui */

#endif /* MODULE_APPS_WINDOWS_APPWINDOW_HPP_ */
