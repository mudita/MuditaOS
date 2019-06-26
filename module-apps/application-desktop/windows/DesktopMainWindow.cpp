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
	// TODO Auto-generated destructor stub
}

void DesktopMainWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
//	for( int i = 0; i<4; i++ )
//		dots[i]->setVisible(false);
}

} /* namespace gui */
