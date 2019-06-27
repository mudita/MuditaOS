/*
 * @file PinWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PinWindow.hpp"

namespace gui {

PinWindow::PinWindow( app::Application* app ) : AppWindow(app, "PinWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setText( BottomBar::Side::LEFT, "SHOW");
	bottomBar->setText( BottomBar::Side::CENTER, "CONFIRM");
	bottomBar->setText( BottomBar::Side::RIGHT, "BACK");

//	hourLabel = new gui::Label(this, 100+xOffset,300-160+yOffset,280,150);
//	hourLabel->setFilled( false );
//	hourLabel->setBorderColor( gui::ColorNoColor );
//	hourLabel->setFont("gt_pressura_regular_140");
//	hourLabel->setText("00");
//	hourLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
//
//	minuteLabel = new gui::Label(this, 100+xOffset,310+yOffset,280,150);
//	minuteLabel->setFilled( false );
//	minuteLabel->setBorderColor( gui::ColorFullBlack );
//	minuteLabel->setFont("gt_pressura_regular_140");
//	minuteLabel->setText("00");
//	minuteLabel->setRadius( 20 );
//	minuteLabel->setPenWidth(1);
//	minuteLabel->setFillColor( gui::Color(10,0));
//	minuteLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
//
//	progressBar = new gui::Progress(this, 480/2-90+xOffset, 300-6+yOffset, 180, 12 );
//	progressBar->setTotalProgress(59);
//	progressBar->setCurrentProgress(0);

}

PinWindow::~PinWindow() {
	// TODO Auto-generated destructor stub
}

void PinWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
//	for( int i = 0; i<4; i++ )
//		dots[i]->setVisible(false);
}

} /* namespace gui */
