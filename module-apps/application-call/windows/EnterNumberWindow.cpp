/*
 * @file EnterNumberWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "../data/CallSwitchData.hpp"
#include "EnterNumberWindow.hpp"
#include "../ApplicationCall.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"

namespace gui {

EnterNumberWindow::EnterNumberWindow( app::Application* app, std::string windowName ) : AppWindow(app, windowName ) {
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, true );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("app_call_call"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );
	topBar->setActive( gui::TopBar::Elements::LOCK, false );

	numberLabel = new gui::Label(this, 54, 145, 375, 100);
	numberLabel->setFilled( false );
	numberLabel->setBorderColor( gui::ColorFullBlack );
	numberLabel->setPenWidth(2);
	numberLabel->setFont("gt_pressura_light_44");
	numberLabel->setText("");
	numberLabel->setMargins( Margins(0,0,0,16));
	numberLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	numberLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}

EnterNumberWindow::~EnterNumberWindow() {
}

bool EnterNumberWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	//process only if key is short released
//	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
//	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
//		return true;

	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {
		if(inputEvent.keyCode == KeyCode::KEY_RF) {
			auto app = reinterpret_cast<app::ApplicationCall*>( application );

			std::string num = app->getDisplayedNumber();
			//if there isn't any char in phone number field return to previous application
			if( num.empty() ) {
				sapm::ApplicationManager::messageSwitchPreviousApplication( application );
				return true;
			}

			num = num.substr(0, num.size()-1);
			if( num.empty())
				bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

			numberLabel->setText(num);
			app->setDisplayedNumber(num);

			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
		}
		//if numeric key was pressed record that key and send it to call application with a switch command
		else if(( inputEvent.keyChar >= '0') && ( inputEvent.keyChar <= '9') ) {

			auto app = reinterpret_cast<app::ApplicationCall*>( application );
			char key[] = { char(inputEvent.keyChar) ,0};
			std::string num = app->getDisplayedNumber();

			if( !num.empty())
				bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

			num += key;
			numberLabel->setText(num);
			app->setDisplayedNumber(num);

			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

			return true;
		}
	}
	else if( inputEvent.state == InputEvent::State::keyReleasedLong) {
		//erase all characters from phone number
		if(inputEvent.keyCode == KeyCode::KEY_RF) {
			auto app = reinterpret_cast<app::ApplicationCall*>( application );

			std::string num = app->getDisplayedNumber();
			//if there isn't any char in phone number field return to previous application
			if( num.empty() ) {
				sapm::ApplicationManager::messageSwitchPreviousApplication( application );
				return true;
			}
			bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

			numberLabel->setText("");
			app->setDisplayedNumber("");

			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
		}
	}
	return false;
}

bool EnterNumberWindow::handleSwitchData( SwitchData* data ) {
	app::CallSwitchData *callData = reinterpret_cast<app::CallSwitchData*>(data);
	switch( callData->getType()) {
		case app::CallSwitchData::Type::UNDEFINED: {
			return false;
		}break;
		case app::CallSwitchData::Type::NUMBER: {
			app::CallNumberData* numberData = reinterpret_cast<app::CallNumberData*>(data);
			numberLabel->setText(numberData->getPhoneNumber());
			auto app = reinterpret_cast<app::ApplicationCall*>( application );
			app->setDisplayedNumber(numberData->getPhoneNumber());
			return true;
		}break;
		case app::CallSwitchData::Type::INCOMMING_CALL: {
			return false;
		}break;
	};
	return false;
}

} /* namespace gui */
