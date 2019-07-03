/*
 * @file CalendarMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALENDAR_WINDOWS_CALENDARMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_CALENDAR_WINDOWS_CALENDARMAINWINDOW_HPP_

#include "windows/AppWindow.hpp"
#include "Application.hpp"

namespace app {

/*
 *
 */
class CalendarMainWindow: public gui::AppWindow {
protected:
	//labels with names of the days
//	gui::Label days;
public:
	CalendarMainWindow( Application* app, std::string name );
	virtual ~CalendarMainWindow();
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALENDAR_WINDOWS_CALENDARMAINWINDOW_HPP_ */
