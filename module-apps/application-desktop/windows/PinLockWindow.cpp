/*
 * @file PinLockWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
//modulr-gui
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

//module-utils
#include "i18/i18.hpp"

#include "PinLockWindow.hpp"

namespace gui {

PinLockWindow::PinLockWindow( app::Application* app ) : AppWindow(app, "PinLockWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, true );
	bottomBar->setActive( BottomBar::Side::CENTER, false );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );

	lockImage = new gui::Image( this, 177,132,0,0, "pin_lock" );
	infoImage = new gui::Image( this, 177,132,0,0, "pin_lock_info" );
	infoImage->setVisible(false);

	//labels with stars for displaying entered digits
	uint32_t pinLabelX = 82;
	for( uint32_t i=0; i<4; i++ ){
		pinLabels[i] = new gui::Label(this, pinLabelX, 442-100, 63, 100);
		pinLabels[i]->setFilled( false );
		pinLabels[i]->setBorderColor( gui::ColorFullBlack );
		pinLabels[i]->setPenWidth(2);
		pinLabels[i]->setFont("gt_pressura_regular_65");
		pinLabels[i]->setText("*");
		pinLabels[i]->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
		pinLabels[i]->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		pinLabelX += 84;
	}

	//labels with stars for displaying entered digits
	uint32_t infoLabelY = 316-22;
	for( uint32_t i=0; i<3; i++ ){
		infoLabels[i] = new gui::Label(this, 0, infoLabelY, 480, 30);
		infoLabels[i]->setFilled( false );
		infoLabels[i]->setBorderColor( gui::ColorNoColor );
		infoLabels[i]->setFont("gt_pressura_regular_24");
		infoLabels[i]->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		infoLabelY += 40;
	}

	infoLabels[0]->setText( utils::localize.get("app_desktop_pin_lock") );
	infoLabels[1]->setText( utils::localize.get("app_desktop_pin_info2") );
}

PinLockWindow::~PinLockWindow() {
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
		infoLabels[2]->setVisible(false);
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_lock"));
		infoLabels[1]->setText("");
		infoLabels[2]->setText("");

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
		infoLabels[0]->setVisible(true);
		infoLabels[1]->setVisible(true);
		infoLabels[2]->setVisible(true);
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_info1"));
		infoLabels[1]->setText(utils::localize.get("app_desktop_pin_info2"));
		infoLabels[2]->setText(std::to_string(remainingAttempts));

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
		infoLabels[0]->setVisible(true);
		infoLabels[1]->setVisible(false);
		infoLabels[2]->setVisible(false);
		infoLabels[0]->setText(utils::localize.get("app_desktop_pin_blocked1"));
		infoLabels[1]->setText("");
		infoLabels[2]->setText("");

		//show pin icon
		lockImage->setVisible(false);
		infoImage->setVisible(true);
	}
}

void PinLockWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {

	//change kbd profile to home_screen
	application->setKeyboardProfile("home_screen");
	if( mode == ShowMode::GUI_SHOW_INIT ) {
	}

	//erase previous charcters and set count of entered characters to 0
	bottomBar->setActive( BottomBar::Side::CENTER, false );
	charCount = 0;
	for( uint32_t i=0; i<4; i++ ) {
		charValue[i] = 0;
		pinLabels[i]->setText("");
	}

	//set state
	setVisibleState( state );
}

bool PinLockWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	if( inputEvent.state == gui::InputEvent::State::keyReleasedShort ) {
		//accept only LF, enter, RF, #, and numeric values
		if( state == State::EnteringPin ) {
			if( inputEvent.keyCode == KeyCode::KEY_LF ) {
				return true;
			}
			else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
				application->switchWindow( "MainWindow", 0, nullptr );
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

					//TODO make pin chacking here, currentyly it always fails

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
					charValue[charCount] = inputEvent.keyChar;
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
				application->switchWindow( "MainWindow", 0, nullptr );
			}
		}
		else if( state == State::PhoneBlocked) {
			if( inputEvent.keyCode == KeyCode::KEY_RF ) {
				application->switchWindow( "MainWindow", 0, nullptr );
				return true;
			}
		}
	}

	return false;
}

} /* namespace gui */
