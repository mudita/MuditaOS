/*
 * @file DesktopMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "../ApplicationDesktop.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "DesktopMainWindow.hpp"
#include "gui/widgets/Image.hpp"

//application-call
#include "application-call/data/CallSwitchData.hpp"


#include "i18/i18.hpp"


namespace gui {

void DesktopMainWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));

	callsImage = new gui::Image( this, 28,266,0,0, "phone" );
	messagesImage = new gui::Image( this, 28, 341,0,0, "mail" );

	time = new gui::Label(this, 34, 90, 250, 116 );
	time->setFilled( false );
	time->setBorderColor( gui::ColorNoColor );
	time->setFont("gt_pressura_light_84");
	time->setText("12:07");
	time->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	dayText = new gui::Label(this, 264, 108, 190, 42 );
	dayText->setFilled( false );
	dayText->setBorderColor( gui::ColorNoColor );
	dayText->setFont("gt_pressura_light_24");
	dayText->setText(utils::localize.get("common_wendesday"));
	dayText->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	dayMonth = new gui::Label(this, 264, 150, 190, 42 );
	dayMonth->setFilled( false );
	dayMonth->setBorderColor( gui::ColorNoColor );
	dayMonth->setFont("gt_pressura_light_24");
	dayMonth->setText("01 Jan");
	dayMonth->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_TOP));

	notificationCalls = new gui::Label(this, 86, 255, 390, 42 );
	notificationCalls->setFilled( false );
	notificationCalls->setBorderColor( gui::ColorNoColor );
	notificationCalls->setFont("gt_pressura_light_24");
	UTF8 calls = "2 " + utils::localize.get("app_desktop_missed_calls");
	notificationCalls->setText(calls);
	notificationCalls->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	notificationMessages = new gui::Label(this, 86, 330, 390, 42 );
	notificationMessages->setFilled( false );
	notificationMessages->setBorderColor( gui::ColorNoColor );
	notificationMessages->setFont("gt_pressura_light_24");
	UTF8 mess = "2 " + utils::localize.get("app_desktop_unread_messages");
	notificationMessages->setText(mess);
	notificationMessages->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}

void DesktopMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete description;
	delete time;
	delete dayText;
	delete dayMonth;
	delete notificationCalls;
	delete notificationMessages;
	delete callsImage;
	delete messagesImage;
	focusItem = nullptr;
	children.clear();
}

DesktopMainWindow::DesktopMainWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );
	buildInterface();
}

DesktopMainWindow::~DesktopMainWindow() {
	destroyInterface();
}

//method hides or show widgets and sets bars according to provided state
void DesktopMainWindow::setVisibleState() {
	if( screenLocked ) {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
		topBar->setActive( TopBar::Elements::LOCK, true );
	}
	else {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
		topBar->setActive( TopBar::Elements::LOCK, false );
	}
}

void DesktopMainWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {

	app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
	pinLockScreen = ( app->getPinLocked() != 0 );
	screenLocked = app->getScreenLocked();

	setVisibleState();
}

bool DesktopMainWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	//process shortpress
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {

		if( screenLocked ) {
			//if enter was pressed
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				unlockStartTime = xTaskGetTickCount();
				enterPressed = true;
			}
			else if(( inputEvent.keyCode == KeyCode::KEY_PND ) && enterPressed ) {
				//if interval between enter and pnd keys is less than time defined for unlocking
				if( xTaskGetTickCount() - unlockStartTime  < unclockTime) {
					//display pin lock screen or simply refresh current window to update labels
					if( pinLockScreen )
						application->switchWindow( "PinLockWindow", 0, nullptr );
					else {
						setVisibleState();
						application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
					}
				}
				enterPressed = false;
			}
			else {
				enterPressed = false;
			}
		}
		//screen is unlocked
		else {
			//pressing enter moves user to menu screen
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				application->switchWindow( "MenuWindow", 0, nullptr );
			}
			//if numeric key was pressed record that key and send it to call application with a switch command
			else if(( inputEvent.keyChar >= '0') && ( inputEvent.keyChar <= '9') ) {

				char key[] = { char(inputEvent.keyChar) ,0};
				std::unique_ptr<gui::SwitchData> phoneNumberData = std::make_unique<app::CallNumberData>(std::string(key));
				sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationCall", "EnterNumberWindow", std::move(phoneNumberData) );

				return true;
			}
		}
	}
	else if( inputEvent.state == InputEvent::State::keyReleasedLong ) {
		//long press of # locks screen if it was unlocked
		if( (inputEvent.keyCode == KeyCode::KEY_PND) && ( screenLocked == false ) ) {
			app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
			app->setScreenLocked(true);
			screenLocked = true;
			setVisibleState();
			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
		}
		//long press of right function button muve user to power off window
		else if (inputEvent.keyCode == KeyCode::KEY_RF) {
			application->switchWindow( "PowerOffWindow", 0, nullptr );
		}
	}
	return false;
}

void DesktopMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

} /* namespace gui */
