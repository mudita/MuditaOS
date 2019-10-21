/*
 * @file PinLockWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
//application manager
#include "service-appmgr/ApplicationManager.hpp"

//module-gui
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

//module-utils
#include "i18/i18.hpp"

#include "PinLockWindow.hpp"

#include "../ApplicationDesktop.hpp"
#include "../data/LockPhoneData.hpp"

namespace gui {

PinLockWindow::PinLockWindow( app::Application* app ) : AppWindow(app, "PinLockWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void PinLockWindow::rebuild() {
	//find which widget has focus
	destroyInterface();
	buildInterface();
	//set state
	focusItem = nullptr;
	setVisibleState( state );
}
void PinLockWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::LEFT, true );
	bottomBar->setActive( BottomBar::Side::CENTER, false );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::SIGNAL, true  );
	topBar->setActive(TopBar::Elements::BATTERY, true  );
	topBar->setActive(TopBar::Elements::LOCK, true  );

	lockImage = new gui::Image( this, 177,132,0,0, "pin_lock" );
	infoImage = new gui::Image( this, 177,132,0,0, "pin_lock_info" );
	infoImage->setVisible(false);

	//title label
	titleLabel = new gui::Label(this, 0, 60, 480, 40);
	titleLabel->setFilled( false );
	titleLabel->setVisible(false);
	titleLabel->setBorderColor( gui::ColorFullBlack );
	titleLabel->setFont("gt_pressura_regular_24");
	titleLabel->setText(utils::localize.get("app_desktop_pin_info1"));
	titleLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	titleLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	//labels with stars for displaying entered digits
	uint32_t pinLabelX = 82;
	for( uint32_t i=0; i<4; i++ ){
		gui::Label* label = new gui::Label(this, pinLabelX, 442-100, 63, 100);
		label->setFilled( false );
		label->setBorderColor( gui::ColorFullBlack );
		label->setPenWidth(2);
		label->setFont("gt_pressura_regular_65");
		label->setText("*");
		label->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
		label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		pinLabels.push_back( label );
		pinLabelX += 84;
	}

	//labels with stars for displaying entered digits
	uint32_t infoLabelY = 316-22;
	for( uint32_t i=0; i<2; i++ ){
		gui::Label* label = new gui::Label(this, 0, infoLabelY, 480, 30);
		label->setFilled( false );
		label->setBorderColor( gui::ColorNoColor );
		label->setFont("gt_pressura_regular_24");
		label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		infoLabels.push_back( label );
		infoLabelY += 40;
	}

	infoLabels[0]->setText( utils::localize.get("app_desktop_pin_lock") );
	infoLabels[1]->setText( utils::localize.get("app_desktop_pin_info2") );
}
void PinLockWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete titleLabel;
	for( uint32_t i=0; i<infoLabels.size(); i++ )
		delete infoLabels[i];
	infoLabels.clear();
	for( uint32_t i=0; i<pinLabels.size(); i++ )
		delete pinLabels[i];
	pinLabels.clear();
	delete lockImage;
	delete infoImage;
	focusItem = nullptr;
	children.clear();
}

PinLockWindow::~PinLockWindow() {
	destroyInterface();
}

void PinLockWindow::setVisibleState( const State& state ) {

	this->state = state;
	//show pin labels for entering pin and erase their's content
	if( state == State::EnteringPin ) {
		charCount = 0;
		for( uint32_t i=0; i<4; i++ ) {
			pinLabels[i]->setVisible(true);
			pinLabels[i]->setText("");
		}
		//hide second info label
		infoLabels[0]->setVisible(true);
		infoLabels[1]->setVisible(false);
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_lock"));
		infoLabels[1]->setText("");

		//show pin icon
		lockImage->setVisible(true);
		infoImage->setVisible(false);

	}
	else if( state == State::WrongPinInfo ) {
		for( uint32_t i=0; i<4; i++ ) {
			pinLabels[i]->setVisible(false);
			pinLabels[i]->setText("");
		}
		//hide second info label
		titleLabel->setVisible(true);
		infoLabels[0]->setVisible(true);
		infoLabels[1]->setVisible(true);
		titleLabel->setText(utils::localize.get("app_desktop_pin_info1"));
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_info2"));
		infoLabels[1]->setText(std::to_string(remainingAttempts));

		//show pin icon
		lockImage->setVisible(false);
		infoImage->setVisible(true);
	}
	else if( state == State::PhoneBlocked ) {
		for( uint32_t i=0; i<4; i++ ) {
			pinLabels[i]->setVisible(false);
			pinLabels[i]->setText("");
		}
		//hide second info label
		titleLabel->setVisible(false);
		infoLabels[0]->setVisible(true);
		infoLabels[1]->setVisible(false);
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_blocked1"));
		infoLabels[1]->setText("");

		//show pin icon
		lockImage->setVisible(false);
		infoImage->setVisible(true);
	}
}

void PinLockWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {

	//check if there was a signal to lock the phone due to inactivity.
	if( (data != nullptr) && (data->getDescription() == "LockPhoneData")) {
		LockPhoneData* lockData = reinterpret_cast<LockPhoneData*>( data );
		lockTimeoutApplilcation = lockData->getPreviousApplication();
	}

	//change kbd profile to home_screen
	application->setKeyboardProfile("home_screen");
	//set state
	setVisibleState( state );
}

bool PinLockWindow::onInput( const InputEvent& inputEvent ) {
	if( inputEvent.state == gui::InputEvent::State::keyReleasedShort ) {
		//accept only LF, enter, RF, #, and numeric values
		if( state == State::EnteringPin ) {
			if( inputEvent.keyCode == KeyCode::KEY_LF ) {
				return true;
			}
			else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
				application->switchWindow( "MainWindow" );
				return true;
			}
			else if( inputEvent.keyCode == KeyCode::KEY_PND ) {
				if( charCount > 0 ) {
					pinLabels[charCount-1]->setText("");
					charCount--;
					bottomBar->setActive( BottomBar::Side::CENTER, false );
					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
				}
				return true;
			}
			else if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				if( (state == State::EnteringPin) && (charCount == 4)) {

					//TODO make pin checking here, currently it always fails
					if( application->getSettings().lockPassHash == (1000*charValue[0]+100*charValue[1]+10*charValue[2]+charValue[3] ) ) {
						remainingAttempts = maxPasswordAttempts;
						app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
						app->setScreenLocked(false);

						//if there is no application to return to simply return to main window
						if( lockTimeoutApplilcation.empty()) {
							application->switchWindow( "MainWindow" );
						}
						else {
							lockTimeoutApplilcation = "";
							sapm::ApplicationManager::messageSwitchPreviousApplication( application );
						}
						return true;
					}

					if( remainingAttempts == 1 ) {
						state = State::PhoneBlocked;
						bottomBar->setActive( BottomBar::Side::CENTER, false );
						setVisibleState( State::PhoneBlocked );
					}
					else {
						remainingAttempts--;
						setVisibleState( State::WrongPinInfo );
					}
					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
				}
				else if( state == State::WrongPinInfo ) {
					setVisibleState( State::EnteringPin );
					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
				}
				return true;
			}
			else if(( inputEvent.keyChar >= '0') && ( inputEvent.keyChar <= '9') ) {
				//fill next field with star and store value in array
				if( charCount < 4 ) {
					pinLabels[charCount]->setText("*");
					charValue[charCount] = inputEvent.keyChar-'0';
					charCount++;

					//if 4 char has been entered show bottom bar confirm
					if( charCount == 4 ) {
						bottomBar->setActive( BottomBar::Side::CENTER, true );
					}
					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
				}
				return true;
			}
		}
		else if( state == State::WrongPinInfo ) {
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				state = State::EnteringPin;
				setVisibleState( State::EnteringPin );
				application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
			}
			else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
				state = State::EnteringPin;
				application->switchWindow( "MainWindow" );
			}
		}
		else if( state == State::PhoneBlocked) {
			if( inputEvent.keyCode == KeyCode::KEY_RF ) {
				application->switchWindow( "MainWindow" );
				return true;
			}
		}
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

} /* namespace gui */
