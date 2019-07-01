/*
 * @file EnterNumberWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_WINDOWS_ENTERNUMBERWINDOW_HPP_
#define MODULE_APPS_APPLICATION_CALL_WINDOWS_ENTERNUMBERWINDOW_HPP_

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui {

/*
 *
 */
class EnterNumberWindow: public AppWindow {
protected:
	gui::BottomBar* bottomBar = nullptr;
	gui::TopBar* topBar = nullptr;
	gui::Label* numberLabel = nullptr;

public:
	EnterNumberWindow( app::Application* app );
	virtual ~EnterNumberWindow();
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_CALL_WINDOWS_ENTERNUMBERWINDOW_HPP_ */
