/*
 * @file CallMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLMAINWINDOW_HPP_

#include "AppWindow.hpp"
#include "Label.hpp"

namespace gui {

/*
 * This window is defined only to keep common approach to creating application.
 * This window should not be displayed.
 */
class CallMainWindow: public AppWindow {
protected:
public:
	CallMainWindow( app::Application* app );
	virtual ~CallMainWindow();

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLMAINWINDOW_HPP_ */
