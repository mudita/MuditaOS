/*
 * @file CalendarMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "CalendarMainWindow.hpp"

namespace app {

CalendarMainWindow::CalendarMainWindow( std::string name ) : Window( name ) {

//	gui::VBox* vBox = new gui::VBox( this, 0, 0, 480, 600 );
//
//	gui::Label title = new gui::Label(vBox, 0,0,480,600);
//	timeLabel->setFont("gt_pressura_bold_65");
//	timeLabel->setText("12:35");
//	timeLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
//	timeLabel->setMaxSize( 480, 180 );
//
//	vBox->addWidget(maxH1);
//	vBox->addWidget(timeLabel);
//	vBox->addWidget(maxH2);
}

CalendarMainWindow::~CalendarMainWindow() {
	// TODO Auto-generated destructor stub
}

} /* namespace app */
