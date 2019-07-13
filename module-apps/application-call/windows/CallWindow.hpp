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
protected:
	enum class State {
		IDLE,
		RINGING,
		CALLING,
		CALL_IN_PROGRESS,
		CALL_ENDED
	};

public:
	CallWindow( app::Application* app );
	virtual ~CallWindow();

	//virtual methods
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_CALL_WINDOWS_CALLWINDOW_HPP_ */
