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
#include "Alignment.hpp"
#include "DesktopMainWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "service-appmgr/ApplicationManager.hpp"

//application-call
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include "i18/i18.hpp"
#include <time/time_conversion.hpp>
#include <Style.hpp>

namespace gui {

    void DesktopMainWindow::buildInterface()
    {

        auto design_time_offset = 106;
        auto design_time_h = 96;
        auto ttime = utils::time::SysTime();
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
        topBar->setActive({{TopBar::Elements::SIGNAL, true}, {TopBar::Elements::LOCK, true}, {TopBar::Elements::BATTERY, true}});

        time = new gui::Label(this, 0, design_time_offset, style::window_width, design_time_h);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(style::window::font::supersizeme);
        time->setText(ttime);
        time->setAlignement(Alignment::ALIGN_HORIZONTAL_CENTER);

        auto design_day_offset = 204;
        auto design_day_h = 51;
        dayText = new gui::Label(this, 0, design_day_offset, style::window_width, design_day_h);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setText(ttime.day() + ", " + ttime.str("%d %b"));
        dayText->setAlignement(Alignment::ALIGN_HORIZONTAL_CENTER);
    }

void DesktopMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete time;
	delete dayText;
	focusItem = nullptr;
	children.clear();
}

DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
{
	buildInterface();
}

DesktopMainWindow::~DesktopMainWindow() {
	destroyInterface();
}

//method hides or show widgets and sets bars according to provided state
void DesktopMainWindow::setVisibleState() {

    auto app = dynamic_cast<app::ApplicationDesktop *>(application);
    if (!app)
    {
        return;
    }

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
        auto app = dynamic_cast<app::ApplicationDesktop *>(application);
        if (!app)
        {
            return;
        }
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
                sapm::ApplicationManager::messageSwitchApplication(application, app::name_call, "EnterNumberWindow", std::move(phoneNumberData));
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
