/*
 * @file PowerOff.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

//module-utils
#include "i18/i18.hpp"

#include "PowerOffWindow.hpp"
#include "../ApplicationDesktop.hpp"

namespace gui {

PowerOffWindow::PowerOffWindow( app::Application* app ) : AppWindow(app, "PowerOffWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	powerImage = new gui::Image( this, 177,132,0,0, "pin_lock_info" );

	//title label
	titleLabel = new gui::Label(this, 0, 60, 480, 40);
	titleLabel->setFilled( false );
	titleLabel->setBorderColor( gui::ColorFullBlack );
	titleLabel->setFont("gt_pressura_regular_24");
	titleLabel->setText(utils::localize.get("app_desktop_poweroff_title"));
	titleLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	titleLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	//label with question for powering down
	infoLabel = new gui::Label(this, 0, 294, 480, 30);
	infoLabel->setFilled( false );
	infoLabel->setBorderColor( gui::ColorNoColor );
	infoLabel->setFont("gt_pressura_regular_24");
	infoLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	infoLabel->setText( utils::localize.get("app_desktop_poweroff_question") );

	uint32_t pinLabelX = 46;
	for( uint32_t i=0; i<4; i++ ){
		selectionLabels[i] = new gui::Label(this, pinLabelX, 397, 193, 75);
		selectionLabels[i]->setFilled( false );
		selectionLabels[i]->setBorderColor( gui::ColorFullBlack );
		selectionLabels[i]->setPenWidth(0);
		selectionLabels[i]->setPenFocusWidth(2);
		selectionLabels[i]->setRadius(5);
		selectionLabels[i]->setFont("gt_pressura_regular_24");
		selectionLabels[i]->setEdges( RectangleEdgeFlags::GUI_RECT_ALL_EDGES );
		selectionLabels[i]->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
		pinLabelX += 193;
	}
	selectionLabels[0]->setText( utils::localize.get("common_no") );
	selectionLabels[1]->setText( utils::localize.get("common_yes") );

	//define navigation between labels
}

PowerOffWindow::~PowerOffWindow() {
}

void PowerOffWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	//on entering screen always set default result as returning to home screen and set focus to "No" label
	state = State::Return;
	setFocusItem( selectionLabels[0] );

}

bool PowerOffWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	//proccess only short press, consume rest
	if( inputEvent.state != gui::InputEvent::State::keyReleasedShort )
		return true;

	if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		application->switchWindow( "MainWindow", 0, nullptr );
	}
	//if enter was pressed check state and power down or return to main desktop's window
	else if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
		if( state == State::PowerDown ){
			//TODO start powering down procedure
		}
		else {
			application->switchWindow( "MainWindow", 0, nullptr );
		}
	}

//	if( inputEvent.state == gui::InputEvent::State::keyReleasedShort ) {
//		//accept only LF, enter, RF, #, and numeric values
//		if( state == State::EnteringPin ) {
//			if( inputEvent.keyCode == KeyCode::KEY_LF ) {
//				return true;
//			}
//			else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
//				application->switchWindow( "MainWindow", 0, nullptr );
//				return true;
//			}
//			else if( inputEvent.keyCode == KeyCode::KEY_PND ) {
//				if( charCount > 0 ) {
//					pinLabels[charCount-1]->setText("");
//					charCount--;
//					bottomBar->setActive( BottomBar::Side::CENTER, false );
//					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
//				}
//				return true;
//			}
//			else if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
//				if( (state == State::EnteringPin) && (charCount == 4)) {
//
//					//TODO make pin chacking here, currentyly it always fails
//					if( application->getSettings().lockPassHash == (1000*charValue[0]+100*charValue[1]+10*charValue[2]+charValue[3] ) ) {
//						remainingAttempts = maxPasswordAttempts;
//						app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
//						app->setScreenLocked(false);
//						application->switchWindow("MainWindow", 0, nullptr );
//						return true;
//					}
//
//					if( remainingAttempts == 1 ) {
//						state = State::PhoneBlocked;
//						bottomBar->setActive( BottomBar::Side::CENTER, false );
//						setVisibleState( State::PhoneBlocked );
//					}
//					else {
//						remainingAttempts--;
//						setVisibleState( State::WrongPinInfo );
//					}
//					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
//				}
//				else if( state == State::WrongPinInfo ) {
//					setVisibleState( State::EnteringPin );
//					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
//				}
//				return true;
//			}
//			else if(( inputEvent.keyChar >= '0') && ( inputEvent.keyChar <= '9') ) {
//				//fill next field with star and store value in array
//				if( charCount < 4 ) {
//					pinLabels[charCount]->setText("*");
//					charValue[charCount] = inputEvent.keyChar-'0';
//					charCount++;
//
//					//if 4 char has been entered show bottom bar confirm
//					if( charCount == 4 ) {
//						bottomBar->setActive( BottomBar::Side::CENTER, true );
//					}
//					application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
//				}
//				return true;
//			}
//		}
//		else if( state == State::WrongPinInfo ) {
//			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
//				state = State::EnteringPin;
//				setVisibleState( State::EnteringPin );
//				application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
//			}
//			else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
//				state = State::EnteringPin;
//				application->switchWindow( "MainWindow", 0, nullptr );
//			}
//		}
//		else if( state == State::PhoneBlocked) {
//			if( inputEvent.keyCode == KeyCode::KEY_RF ) {
//				application->switchWindow( "MainWindow", 0, nullptr );
//				return true;
//			}
//		}
//	}

	return false;
}

} /* namespace gui */
