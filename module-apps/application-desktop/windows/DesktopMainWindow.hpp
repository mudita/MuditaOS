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
	gui::Label* description = nullptr;
	gui::Label* time = nullptr;
	gui::Label* dayText = nullptr;
	gui::Label* dayMonth = nullptr;
	gui::Label* notificationCalls = nullptr;
	gui::Label* notificationMessages = nullptr;
	gui::Image* callsImage = nullptr;
	gui::Image* messagesImage = nullptr;
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
//	bool screenLocked = true;
	/**
	 * Flag that defines if pin window should be displayed
	 */
//	bool pinLockScreen = false;
	//method hides or show widgets and sets bars according to provided state
	void setVisibleState();
public:
	DesktopMainWindow( app::Application* app );
	virtual ~DesktopMainWindow();

	//virtual methods gui::Window
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	//virtual methods gui::AppWindow
	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_DESKTOPMAINWINDOW_HPP_ */
