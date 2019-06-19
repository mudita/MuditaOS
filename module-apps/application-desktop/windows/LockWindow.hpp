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

#include <module-gui/gui/widgets/Window.hpp>

namespace gui {

/*
 *
 */
class LockWindow: public Window {
public:
	LockWindow();
	virtual ~LockWindow();
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_LOCKWINDOW_HPP_ */
