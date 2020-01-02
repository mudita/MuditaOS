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
#include "../data/LockPhoneData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "DesktopMainWindow.hpp"
#include "gui/widgets/Image.hpp"

//application-call
#include "application-call/data/CallSwitchData.hpp"

#include "i18/i18.hpp"
#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui {

void DesktopMainWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));

	topBar->setActive(TopBar::Elements::SIGNAL, true );
	topBar->setActive(TopBar::Elements::LOCK, true );
	topBar->setActive(TopBar::Elements::BATTERY, true );

	callsImage = new gui::Image( this, 28,258,0,0, "phone" );
	messagesImage = new gui::Image( this, 28, 333,0,0, "mail" );

	time = new gui::Label(this, 20, 90, 280, 116 );
	time->setFilled( false );
	time->setBorderColor( gui::ColorNoColor );
	time->setFont(style::window::font::verybig);
	time->setText("12:07");
	time->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	dayText = new gui::Label(this, 264, 108, 190, 42 );
	dayText->setFilled( false );
	dayText->setBorderColor( gui::ColorNoColor );
	dayText->setFont(style::window::font::medium);
    auto time = utils::time::SysTime();
	dayText->setText(time.day());
	dayText->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	dayMonth = new gui::Label(this, 264, 150, 190, 42 );
	dayMonth->setFilled( false );
	dayMonth->setBorderColor( gui::ColorNoColor );
	dayMonth->setFont(style::window::font::medium);
	dayMonth->setText(time.str("%d %B"));
	dayMonth->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_TOP));

	notificationCalls = new gui::Text(this, 86, 255, 350, 70 );
	notificationCalls->setFilled( false );
	notificationCalls->setBorderColor( gui::ColorNoColor );
	notificationCalls->setFont(style::window::font::medium);
	UTF8 calls = "2 " + utils::localize.get("app_desktop_missed_calls");
	notificationCalls->setText(calls);


	notificationMessages = new gui::Text(this, 86, 330, 350, 70 );
	notificationMessages->setFilled( false );
	notificationMessages->setBorderColor( gui::ColorNoColor );
	notificationMessages->setFont(style::window::font::medium);
	UTF8 mess = "2 " + utils::localize.get("app_desktop_unread_messages");

	notificationMessages->setText(mess);
}

void DesktopMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
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

DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
{
    setSize( 480, 600 );
	buildInterface();
}

DesktopMainWindow::~DesktopMainWindow() {
	destroyInterface();
}

//method hides or show widgets and sets bars according to provided state
void DesktopMainWindow::setVisibleState() {

	app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );

	if( app->getScreenLocked() ) {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
		topBar->setActive( TopBar::Elements::LOCK, true );
	}
	else {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
		topBar->setActive( TopBar::Elements::LOCK, false );
	}
}

void DesktopMainWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {

	//update time
	time->setText( topBar->getTimeString() );

	//check if there was a signal to lock the pone due to inactivity.
	if( (data != nullptr) && (data->getDescription() == "LockPhoneData")) {
		app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
		app->setScreenLocked(true);

		LockPhoneData* lockData = reinterpret_cast<LockPhoneData*>( data );
		lockTimeoutApplilcation = lockData->getPreviousApplication();

		reinterpret_cast<app::ApplicationDesktop*>(application)->setSuspendFlag(true);
	}

	setVisibleState();
}

bool DesktopMainWindow::onInput( const InputEvent& inputEvent ) {
	
	// do nothing
	if( inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == KeyCode::KEY_RF ) return false;
	
	app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );

	//process shortpress
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {

		if( app->getScreenLocked() ) {
			//if enter was pressed
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				unlockStartTime = xTaskGetTickCount();
				enterPressed = true;
			}
			else if(( inputEvent.keyCode == KeyCode::KEY_PND ) && enterPressed ) {
				//if interval between enter and pnd keys is less than time defined for unlocking
				if( xTaskGetTickCount() - unlockStartTime  < unclockTime) {
					//display pin lock screen or simply refresh current window to update labels
					if( app->getPinLocked())
						//if there was no application on to before closing proceed normally to pin protection window.
						if( lockTimeoutApplilcation.empty()) {
							application->switchWindow( "PinLockWindow" );
						}
						else {
							std::unique_ptr<LockPhoneData> data = std::make_unique<LockPhoneData>();
							data->setPrevApplication( lockTimeoutApplilcation );
							lockTimeoutApplilcation = "";
							application->switchWindow( "PinLockWindow", std::move(data) );
						}

					else {

						//if phone was locked by user show unlocked main window
						if( lockTimeoutApplilcation.empty() ) {
							app->setScreenLocked(false);
							setVisibleState();
							application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
						}
						//if there was application on top when timeout occurred
						else {
							lockTimeoutApplilcation = "";
							sapm::ApplicationManager::messageSwitchPreviousApplication( application );
						}
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
            int val = gui::toNumeric(inputEvent.keyCode);
			//pressing enter moves user to menu screen
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				application->switchWindow( "MenuWindow" );
			}
			//if numeric key was pressed record that key and send it to call application with a switch command
            else if( 0 <= val && val <= 9)
            {
                auto key = std::to_string(val);
                std::unique_ptr<gui::SwitchData> phoneNumberData = std::make_unique<app::EnterNumberData>(key);
                sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationCall", "EnterNumberWindow", std::move(phoneNumberData) );
                return true;
            }
		}
	}
	else if( inputEvent.state == InputEvent::State::keyReleasedLong ) {
		//long press of # locks screen if it was unlocked
		if( (inputEvent.keyCode == KeyCode::KEY_PND) && ( app->getScreenLocked() == false ) ) {
			app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
			app->setScreenLocked(true);
			setVisibleState();
			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

			reinterpret_cast<app::ApplicationDesktop*>(application)->setSuspendFlag(true);
		}
		//long press of right function button move user to power off window
		else if (inputEvent.keyCode == KeyCode::KEY_RF) {
			application->switchWindow( "PowerOffWindow" );
            return true;
		}
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

void DesktopMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

bool DesktopMainWindow::updateTime( const UTF8& timeStr ) {
	auto ret = AppWindow::updateTime( timeStr );
	time->setText( topBar->getTimeString());
	return ret;
}
bool DesktopMainWindow::updateTime( const uint32_t& timestamp, bool mode24H ) {
	auto ret = AppWindow::updateTime( timestamp, mode24H );
	time->setText( topBar->getTimeString());
	return ret;
}

std::list<DrawCommand*> DesktopMainWindow::buildDrawList() {
	time->setText( topBar->getTimeString());
	return Window::buildDrawList();
}

} /* namespace gui */
