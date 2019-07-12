/*
 * @file CallMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "Label.hpp"
#include "CallMainWindow.hpp"

namespace gui {

CallMainWindow::CallMainWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void CallMainWindow::rebuild() {

}
void CallMainWindow::buildInterface() {
	title = new gui::Label(this, 0, 100, 480, 116 );
	title->setFilled( false );
	title->setBorderColor( gui::ColorNoColor );
	title->setFont("gt_pressura_light_24");
	title->setText(getName());
	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}
void CallMainWindow::destroyInterface() {
}


CallMainWindow::~CallMainWindow() {
}

} /* namespace gui */
