/*
 * @file DesktopMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "DesktopMainWindow.hpp"
#include "gui/widgets/Image.hpp"

#include "i18/i18.hpp"


namespace gui {

DesktopMainWindow::DesktopMainWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, false );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );

	new gui::Image( this, 0,599-50-230,0,0, "mountains" );

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
	dayMonth->setText("04 Jan");
	dayMonth->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_TOP));
}

DesktopMainWindow::~DesktopMainWindow() {
}

void DesktopMainWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {

	//
	if( screenLocked ) {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
		topBar->setActive( TopBar::Elements::LOCK, false );
	}
	else {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
		topBar->setActive( TopBar::Elements::LOCK, false );
	}
}

bool DesktopMainWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	if( screenLocked ) {
		//if enter was pressed
		if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
			unlockStartTime = xTaskGetTickCount();
			enterPressed = true;
		}
		else if(( inputEvent.keyCode == KeyCode::KEY_PND ) && enterPressed ) {
			//if interval between enter and pnd keys is less than time defined for unlocking
			if( xTaskGetTickCount() - unlockStartTime  < unclockTime) {
				screenLocked = false;
				application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
			}
			enterPressed = false;
		}
		else {
			enterPressed = false;
		}
	}
	else {
		//lock screen if it was unlocked
		if( (inputEvent.keyCode == KeyCode::KEY_PND) && (inputEvent.state == InputEvent::State::keyReleasedLong ) ) {
			screenLocked = true;
			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
		}
	}
	return false;
}

} /* namespace gui */
