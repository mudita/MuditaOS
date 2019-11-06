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
#include "service-cellular/api/CellularServiceAPI.hpp"

#include "i18/i18.hpp"
#include <Style.hpp>

namespace gui {

EnterNumberWindow::EnterNumberWindow( app::Application* app, std::string windowName ) : AppWindow(app, windowName ) {
	setSize( 480, 600 );
	buildInterface();
}

void EnterNumberWindow::rebuild() {

}
void EnterNumberWindow::buildInterface() {

	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::LEFT, true );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("app_call_call"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

	topBar->setActive(TopBar::Elements::SIGNAL, true );
	topBar->setActive(TopBar::Elements::BATTERY, true );

	numberLabel = new gui::Label(this, 54, 145, 375, 100);
	numberLabel->setFilled( false );
	numberLabel->setBorderColor( gui::ColorFullBlack );
	numberLabel->setPenWidth(2);
	numberLabel->setFont(style::header::font::title);
	numberLabel->setText("");
	numberLabel->setMargins( Margins(0,0,0,16));
	numberLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	numberLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}
void EnterNumberWindow::destroyInterface() {
	AppWindow::destroyInterface();
	if( numberLabel ) { removeWidget(numberLabel); delete numberLabel; numberLabel= nullptr; }
	children.clear();
}

EnterNumberWindow::~EnterNumberWindow() {
	destroyInterface();
}

bool EnterNumberWindow::onInput( const InputEvent& inputEvent ) {
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {
		if(inputEvent.keyCode == KeyCode::KEY_ENTER) {
			auto app = reinterpret_cast<app::ApplicationCall*>( application );
			std::string num = app->getDisplayedNumber();
			LOG_INFO("number: [%s]", num.c_str());
			auto ret = CellularServiceAPI::DialNumber(application,num.c_str());
			LOG_INFO("CALL RESULT: %s", (ret?"OK":"FAIL"));
		}
		else if(inputEvent.keyCode == KeyCode::KEY_RF) {
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

			return true;
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
			
			return true;
		}
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

bool EnterNumberWindow::handleSwitchData( SwitchData* data ) {

	if( data == nullptr )
	{
		LOG_ERROR("Received null pointer");
		return false;
	}

	app::CallSwitchData *callData = reinterpret_cast<app::CallSwitchData*>(data);
	switch( callData->getType()) {
		case app::CallSwitchData::Type::UNDEFINED: {
			return false;
		}break;
		case app::CallSwitchData::Type::ENTER_NUMBER: {
			app::EnterNumberData* numberData = reinterpret_cast<app::EnterNumberData*>(data);
			numberLabel->setText(numberData->getPhoneNumber());
			auto app = reinterpret_cast<app::ApplicationCall*>( application );
			app->setDisplayedNumber(numberData->getPhoneNumber());
			return true;
		}break;
		case app::CallSwitchData::Type::INCOMMING_CALL: {
			return false;
		}break;
		case app::CallSwitchData::Type::EXECUTE_CALL: {
			LOG_INFO("app::CallSwitchData::Type::EXECUTE_CALL");
			return false;
		}break;
	};
	return false;
}

} /* namespace gui */
