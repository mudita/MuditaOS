/*
 * @file EmergencyCallWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "../data/CallSwitchData.hpp"
#include "../ApplicationCall.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"
#include "EmergencyCallWindow.hpp"

namespace gui {

EmergencyCallWindow::EmergencyCallWindow( app::Application* app ) : EnterNumberWindow( app, "EmergencyCallWindow" ) {

	numberLabel->setText( utils::localize.get("app_call_emergency") );
}

void EmergencyCallWindow::rebuild() {

}
void EmergencyCallWindow::buildInterface() {
	AppWindow::buildInterface();
}
void EmergencyCallWindow::destroyInterface() {
	AppWindow::destroyInterface();
}

EmergencyCallWindow::~EmergencyCallWindow() {
	// TODO Auto-generated destructor stub
}

bool EmergencyCallWindow::onInput( const InputEvent& inputEvent ) {
	bool ret = EnterNumberWindow::onInput( inputEvent );

	if( number.empty() ) {
		numberLabel->setText( utils::localize.get("app_call_emergency") );
	}

	return ret;
}
bool EmergencyCallWindow::handleSwitchData( SwitchData* data ) {
	bool ret = EnterNumberWindow::handleSwitchData( data );

	return ret;
}

} /* namespace gui */
