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

#include <module-gui/gui/widgets/Window.hpp>
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"

namespace gui {

/*
 *
 */
class PinWindow: public Window {
	gui::Label* description = nullptr;
	gui::Image* confirmArrow = nullptr;
	gui::Image* dots[4] = {  nullptr, nullptr, nullptr, nullptr };
public:
	PinWindow();
	virtual ~PinWindow();
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINWINDOW_HPP_ */
