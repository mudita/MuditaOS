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
#include "Application.hpp"

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

uint32_t AppWindow::calculateBatteryLavel( uint32_t percentage ) {
	if( percentage <=5 )
		return 0;
	if( percentage <=27 )
		return 1;
	if( percentage <=50 )
		return 2;
	if( percentage <=73 )
		return 3;
	if( percentage <=95 )
		return 4;

	return 5;
}

//updates battery level in the window
bool AppWindow::updateBatteryLevel( uint32_t percentage ) {
	//get old value of battery level, calcualte new level and comapre both
	//if they are different make a change and return true, otherwise return false;
	uint32_t oldLevel = topBar->getBatteryLevel();
	uint32_t newLevel = calculateBatteryLavel( percentage );
	if( oldLevel != newLevel ) {
		topBar->setBatteryLevel( newLevel );
		return true;
	}

	return false;
}
//updates battery level in the window
bool AppWindow::updateSignalStrength( uint32_t strength ) {

	uint32_t oldStrength = topBar->getSignalStrength();

	uint32_t newStrength = 0;
	if( strength >= 2 ) newStrength = 1;
	if( strength >= 10 ) newStrength = 2;
	if( strength >= 15 ) newStrength = 3;
	if( strength >= 20 ) newStrength = 4;
	if( strength >= 25 ) newStrength = 5;
	if( newStrength != oldStrength ) {
		topBar->setSignalStrength( newStrength );
		return true;
	}
	return false;
}

std::list<DrawCommand*> AppWindow::buildDrawList() {
	//update top bar and bottom bar
	topBar->setBatteryLevel( topBar->getBatteryLevel());
	topBar->setSignalStrength( topBar->getSignalStrength());

	return Window::buildDrawList();
}

} /* namespace gui */
