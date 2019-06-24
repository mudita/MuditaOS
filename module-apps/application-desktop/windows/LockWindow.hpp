/*
 * @file LockWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_LOCKWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_LOCKWINDOW_HPP_

#include "AppWindow.hpp"

namespace gui {

/*
 *
 */
class LockWindow: public AppWindow {
public:
	LockWindow( app::Application* app );
	virtual ~LockWindow();
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_LOCKWINDOW_HPP_ */
