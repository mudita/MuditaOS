/*
 * @file EnterNumberWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "EnterNumberWindow.hpp"

#include "i18/i18.hpp"

namespace gui {

EnterNumberWindow::EnterNumberWindow( app::Application* app ) : AppWindow(app,"EnterNumberWindow") {
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, false );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );

	numberLabel = new gui::Label(this, 54, 245, 372, 100);
	numberLabel->setFilled( false );
	numberLabel->setBorderColor( gui::ColorFullBlack );
	numberLabel->setPenWidth(2);
	numberLabel->setFont("gt_pressura_regular_24");
	numberLabel->setText("606288291");
	numberLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	numberLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}

EnterNumberWindow::~EnterNumberWindow() {
	// TODO Auto-generated destructor stub
}

} /* namespace gui */
