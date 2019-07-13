/*
 * @file AppWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "i18/i18.hpp"
#include "AppWindow.hpp"

namespace gui {

AppWindow::AppWindow( app::Application* app, std::string name, uint32_t id ) :
	Window ( name, id), application{ app } {
}

AppWindow::~AppWindow() {
}


void AppWindow::destroyInterface() {
	children.remove(bottomBar);
	children.remove(topBar);
	delete bottomBar;
	delete topBar;
}

void AppWindow::rebuild() {
}
void AppWindow::buildInterface() {
	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, false );
	bottomBar->setActive( BottomBar::Side::RIGHT, false );

	topBar = new gui::TopBar( this, 0,0, 480, 50 );
	topBar->setActive(TopBar::Elements::LOCK, false );
	topBar->setActive(TopBar::Elements::BATTERY, false );
	topBar->setActive(TopBar::Elements::SIGNAL, false );
}

} /* namespace gui */
