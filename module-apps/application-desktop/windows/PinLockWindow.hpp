/*
 * @file PinLockWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINLOCKWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINLOCKWINDOW_HPP_

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui {

class PinLockWindow: public AppWindow {
	enum class State {
		EnteringPin,
		WrongPinInfo,
		PhoneBlocked
	};

	gui::BottomBar* bottomBar = nullptr;
	gui::TopBar* topBar = nullptr;
	gui::Label* infoLabels[3] = { nullptr, nullptr, nullptr };
	gui::Label* pinLabels[4] = { nullptr, nullptr, nullptr, nullptr };
	gui::Image* lockImage = nullptr;
	gui::Image* infoImage = nullptr;
	uint32_t remainingAttempts = 4;
	//code of the entered character on specified position
	uint32_t charValue[4] = {0};
	//flag defines number of entered characters
	uint32_t charCount = 0;
	//state of the window
	State state = State::EnteringPin;

	//method hides or show widgets and sets bars according to provided state
	void setVisibleState( const State& state );
public:
	PinLockWindow( app::Application* app );
	virtual ~PinLockWindow();
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	bool onInput( const InputEvent& inputEvent ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_PINLOCKWINDOW_HPP_ */
