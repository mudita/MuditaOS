/*
 * @file CallWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationCall.hpp"
#include "../data/CallSwitchData.hpp"

#include "i18/i18.hpp"

#include "service-cellular/api/CellularServiceAPI.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "CallWindow.hpp"

namespace gui {

CallWindow::CallWindow( app::Application* app, std::string windowName ) : AppWindow(app, windowName){
	setSize( 480, 600 );
	buildInterface();
}

void CallWindow::rebuild() {
	destroyInterface();
	buildInterface();
}
void CallWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_select"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));


	titleLabel = new gui::Label(this, 0, 50, 480, 50 );
	titleLabel->setFilled( false );
	titleLabel->setBorderColor( gui::ColorNoColor );
	titleLabel->setFont("gt_pressura_bold_24");
	titleLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	numberLabel = new gui::Label(this, 0, 150, 480, 50 );
	numberLabel->setFilled( false );
	numberLabel->setBorderColor( gui::ColorNoColor );
	numberLabel->setFont("gt_pressura_bold_24");
	numberLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
}

void CallWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete titleLabel;
	delete numberLabel;
	children.remove( titleLabel );
	children.remove( numberLabel );
}

CallWindow::~CallWindow() {
}

void CallWindow::setState( State state ) {
	this->state = state;
	setVisibleState();
}

void CallWindow::setVisibleState() {
	//show state of the window
	switch( state ) {
		case State::INCOMMING_CALL: {
			titleLabel->setText("INCOMMING_CALL");
			bottomBar->setActive(gui::BottomBar::Side::LEFT, true );
			bottomBar->setActive(gui::BottomBar::Side::CENTER, true );
			bottomBar->setActive(gui::BottomBar::Side::RIGHT, true );
			bottomBar->setText( gui::BottomBar::Side::LEFT, "Accept" );
			bottomBar->setText( gui::BottomBar::Side::CENTER, "Message" );
			bottomBar->setText( gui::BottomBar::Side::RIGHT, "Reject" );
		}break;
		case State::CALL_ENDED: {
			titleLabel->setText("CALL_ENDED");

			bottomBar->setActive(gui::BottomBar::Side::LEFT, false );
			bottomBar->setActive(gui::BottomBar::Side::CENTER, false );
			bottomBar->setActive(gui::BottomBar::Side::RIGHT, true );
			bottomBar->setText( gui::BottomBar::Side::RIGHT, "Return" );
		}break;
		case State::CALL_IN_PROGRESS: {
			titleLabel->setText("CALL_IN_PROGRESS");

			bottomBar->setActive(gui::BottomBar::Side::LEFT, false );
			bottomBar->setActive(gui::BottomBar::Side::CENTER, false );
			bottomBar->setActive(gui::BottomBar::Side::RIGHT, true );
			bottomBar->setText( gui::BottomBar::Side::RIGHT, "End Call" );
		}break;
		case State::IDLE: {
			titleLabel->setText("IDLE");
		}break;
		case State::OUTGOING_CALL: {
			titleLabel->setText("OUTGOING_CALL");

			bottomBar->setActive(gui::BottomBar::Side::LEFT, false );
			bottomBar->setActive(gui::BottomBar::Side::CENTER, false );
			bottomBar->setActive(gui::BottomBar::Side::RIGHT, true );
			bottomBar->setText( gui::BottomBar::Side::RIGHT, "End Call" );
		}break;
	};
}

bool CallWindow::handleSwitchData( SwitchData* data ) {

	if( data == nullptr )
		LOG_ERROR("Received null pointer");
		return false;

	app::CallSwitchData* callData = reinterpret_cast<app::CallSwitchData*>(data);
	if( callData->getType() == app::CallSwitchData::Type::INCOMMING_CALL ) {
		app::IncommingCallData* incData = reinterpret_cast<app::IncommingCallData*>( data );
		state = State::INCOMMING_CALL;
		numberLabel->setText( incData->getPhoneNumber());
	}
	setVisibleState();
	application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );

	return true;
}

void CallWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
}

//AnswerIncomingCall
//HangupCall
//auto ret = CellularServiceAPI::DialNumber(this,"");
//		sapm::ApplicationManager::messageSwitchPreviousApplication( application );
bool CallWindow::handleLeftButton() {
	if( state == State::INCOMMING_CALL ) {
		auto ret = CellularServiceAPI::AnswerIncomingCall(application);

		LOG_INFO("AnswerIncomingCall: %s",(ret?"OK":"FAIL"));
		return true;
	}
	else if( state == State::OUTGOING_CALL ) {

	}
	else if( state == State::CALL_ENDED ) {

	}
	else if( state == State::CALL_IN_PROGRESS ) {

	}
	return false;
}
bool CallWindow::handleCenterButton() {
	if( state == State::INCOMMING_CALL ) {
		auto ret = CellularServiceAPI::HangupCall(application);
		LOG_INFO("HangupCall: %s",(ret?"OK":"FAIL"));
		//TODO switch to message templates window
		return true;
	}
	else if( state == State::OUTGOING_CALL ) {

	}
	else if( state == State::CALL_ENDED ) {

	}
	else if( state == State::CALL_IN_PROGRESS ) {

	}
	return false;
}
bool CallWindow::handleRightButton() {
	if( state == State::INCOMMING_CALL ) {
		auto ret = CellularServiceAPI::HangupCall(application);
		LOG_INFO("HangupCall: %s",(ret?"OK":"FAIL"));

		return true;
	}
	else if( state == State::OUTGOING_CALL ) {
		auto ret = CellularServiceAPI::HangupCall(application);
		LOG_INFO("HangupCall: %s",(ret?"OK":"FAIL"));
	}
	else if( state == State::CALL_ENDED ) {
		//return to previous application
		sapm::ApplicationManager::messageSwitchPreviousApplication( application );
		return true;
	}
	else if( state == State::CALL_IN_PROGRESS ) {
		auto ret = CellularServiceAPI::HangupCall(application);
		LOG_INFO("HangupCall: %s",(ret?"OK":"FAIL"));

 		return true;
	}
	return false;
}

bool CallWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER )
			application->render( RefreshModes::GUI_REFRESH_FAST );
		return true;
	}

	bool handled = false;

	//process only if key is released
	if( inputEvent.state != InputEvent::State::keyReleasedShort ) {
		if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
			handled = handleCenterButton();
		}
		else if( inputEvent.keyCode == KeyCode::KEY_LF ) {
			handled = handleLeftButton();
		}
		else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
			handled = handleRightButton();
		}
	}

	if( handled )
		application->refreshWindow( RefreshModes::GUI_REFRESH_FAST);

	return false;
}

} /* namespace gui */

