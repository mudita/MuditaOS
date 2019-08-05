/*
 * @file CallWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLWINDOW_HPP_
#define MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLWINDOW_HPP_

#include "AppWindow.hpp"

namespace gui {

/*
 *
 */
class CallWindow: public AppWindow {
public:
	enum class State {
		IDLE,
		INCOMMING_CALL,
		OUTGOING_CALL,
		CALL_IN_PROGRESS,
		CALL_ENDED
	};
protected:
	gui::Label* titleLabel = nullptr;
	gui::Label* numberLabel = nullptr;
	State state = State::IDLE;
	/**
	 * Manipulates widgets to handle currently set state of the window.
	 */
	void setVisibleState();
	bool handleLeftButton();
	bool handleCenterButton();
	bool handleRightButton();
public:
	CallWindow( app::Application* app, std::string windowName = "CallWindow" );
	virtual ~CallWindow();

	/**
	 * Used by application to update window's state
	 */
	void setState( State state );

	//virtual methods
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	bool handleSwitchData( SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLWINDOW_HPP_ */
