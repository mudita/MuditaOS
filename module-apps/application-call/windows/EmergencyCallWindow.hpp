/*
 * @file EmergencyCallWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_WINDOWS_EMERGENCYCALLWINDOW_HPP_
#define MODULE_APPS_APPLICATION_CALL_WINDOWS_EMERGENCYCALLWINDOW_HPP_

#include "EnterNumberWindow.hpp"

namespace gui {

/*
 *
 */
class EmergencyCallWindow: public EnterNumberWindow {
public:
	EmergencyCallWindow( app::Application* app );
	virtual ~EmergencyCallWindow();

	bool onInput( const InputEvent& inputEvent ) override;
	bool handleSwitchData( SwitchData* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_CALL_WINDOWS_EMERGENCYCALLWINDOW_HPP_ */
