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

#include "gui/widgets/Window.hpp"
#include "Application.hpp"

namespace gui {

/*
 * @brief This is wrapper for gui window used within applications.
 */
class AppWindow: public Window {
protected:
	/**
	 * Pointer to the application object that owns the window.
	 */
	app::Application* application = nullptr;
public:

	AppWindow( app::Application* app, std::string name, uint32_t id=GUIWindowID++ );
	virtual ~AppWindow();

	app::Application* getApplication() { return application; };
	void setApplication( app::Application* app ) { application = app; };
	void rebuild() {};
};

} /* namespace gui */

#endif /* MODULE_APPS_WINDOWS_APPWINDOW_HPP_ */
