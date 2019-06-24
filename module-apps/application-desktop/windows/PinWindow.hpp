/*
 * @file PinWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINWINDOW_HPP_

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui {

/*
 *
 */
class PinWindow: public AppWindow {
	gui::BottomBar* bottomBar;
	gui::Label* description = nullptr;
	gui::Image* confirmArrow = nullptr;
	gui::Image* dots[4] = {  nullptr, nullptr, nullptr, nullptr };
public:
	PinWindow( app::Application* app );
	virtual ~PinWindow();
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINWINDOW_HPP_ */
