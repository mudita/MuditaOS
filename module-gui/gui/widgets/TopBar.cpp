/*
 * @file TopBar.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Image.hpp"
#include "TopBar.hpp"

namespace gui {

uint32_t TopBar::signalStrength = 5;
uint32_t TopBar::batteryLevel = 5;

const uint32_t TopBar::signalOffset = 35;
const uint32_t TopBar::batteryOffset = 415;

TopBar::TopBar() {

	prepareWidget();

	setFillColor( ColorFullWhite );
	setBorderColor( ColorNoColor );
	setFilled(true);
	setSize(480, 50);
}
TopBar::TopBar( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h ) : Rect{ parent, x, y, w, h } {

	prepareWidget();

	setFillColor( ColorFullWhite );
	setBorderColor( ColorNoColor );
	setFilled(true);
	setSize(480, 50);
	updateDrawArea();
}
TopBar::~TopBar() {
}

void TopBar::prepareWidget() {

	signal[0] = new gui::Image( this, signalOffset,17,0,0, "signal0" );
	signal[1] = new gui::Image( this, signalOffset,17,0,0, "signal1" );
	signal[2] = new gui::Image( this, signalOffset,17,0,0, "signal2" );
	signal[3] = new gui::Image( this, signalOffset,17,0,0, "signal3" );
	signal[4] = new gui::Image( this, signalOffset,17,0,0, "signal4" );
	signal[5] = new gui::Image( this, signalOffset,17,0,0, "signal5" );
	for( uint32_t i=0; i<signalImgCount; ++i )
		signal[i]->setVisible( false );

	signal[signalStrength]->setVisible( true );

	//icons for battery
	battery[0] = new gui::Image( this, batteryOffset,17,0,0, "battery0" );
 	battery[1] = new gui::Image( this, batteryOffset,17,0,0, "battery1" );
	battery[2] = new gui::Image( this, batteryOffset,17,0,0, "battery2" );
	battery[3] = new gui::Image( this, batteryOffset,17,0,0, "battery3" );
	battery[4] = new gui::Image( this, batteryOffset,17,0,0, "battery4" );
	battery[5] = new gui::Image( this, batteryOffset,17,0,0, "battery5" );
	for( uint32_t i=0; i<batteryLevelCount; ++i )
		battery[i]->setVisible( false );

	battery[batteryLevel]->setVisible( true );

	//icon of the lock
	lock = new gui::Image( this, 240-11,17,0,0, "lock" );
}

void TopBar::setActive( TopBar::Elements element, bool active ) {
	switch( element ) {
		case Elements::BATTERY: {
			for( uint32_t i=0; i<TopBar::batteryLevelCount; ++i )
				battery[i]->setVisible(false);
			if( active )
				battery[batteryLevel]->setVisible(active);
		} break;
		case Elements::LOCK: {
			lock->setVisible(active);
		} break;
		case Elements::SIGNAL: {
			for( uint32_t i=0; i<TopBar::batteryLevelCount; ++i )
				signal[i]->setVisible(false);
			if( active )
				signal[signalStrength]->setVisible(true);

		} break;
	};
}

void TopBar::setBatteryLevel( uint32_t level ) {
	batteryLevel = level;
	for( uint32_t i=0; i<batteryLevelCount; i++ ) {
		battery[i]->setVisible( false );
	}
	battery[level]->setVisible( true );
}

} /* namespace gui */
