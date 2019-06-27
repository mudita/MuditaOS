/*
 * @file DesktopMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_DESKTOPMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_DESKTOPMAINWINDOW_HPP_

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
class DesktopMainWindow: public AppWindow {
protected:

	gui::BottomBar* bottomBar;
	gui::TopBar* topBar;
	gui::Label* description = nullptr;
	gui::Label* time = nullptr;
	gui::Label* dayText = nullptr;
	gui::Label* dayMonth = nullptr;
	gui::Image* confirmArrow = nullptr;
	gui::Image* dots[4] = {  nullptr, nullptr, nullptr, nullptr };

	/**
	 * Time for pressing sequence of two buttons for unlocking the device in miliseconds.
	 */
	uint32_t unclockTime = 3000;
	/**
	 * value of the system time when enter key was pressed.
	 */
	uint32_t unlockStartTime = 0;
	/**
	 * Flag used in detecting unlock seqience
	 */
	bool enterPressed = false;
	/**
	 * Flag informs whether window has been unlocked
	 */
	bool screenLocked = true;
public:
	DesktopMainWindow( app::Application* app );
	virtual ~DesktopMainWindow();

	//virtual methods
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_DESKTOPMAINWINDOW_HPP_ */
