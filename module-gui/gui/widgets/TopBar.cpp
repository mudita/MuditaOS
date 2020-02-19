/*
 * @file TopBar.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "Style.hpp"

#include "common_data/EventStore.hpp"

namespace gui {

uint32_t TopBar::signalStrength = 0;

const uint32_t TopBar::signalOffset = 35;
const uint32_t TopBar::batteryOffset = 415;
gui::TopBar::TimeMode TopBar::timeMode = TimeMode::TIME_24H;
uint32_t TopBar::time = 0;

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

void TopBar::batteryShowBars(uint32_t val)
{
    int i = 0;
    if (val > battery.size())
    {
        LOG_ERROR("Trying to set battery level out of scope");
        val = battery.size();
    }
    for (int i = 0; i < battery.size(); ++i)
    {
        battery[i]->setVisible(val > i);
    }
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
    battery = {
        new gui::Image(this, batteryOffset, 17, 0, 0, "battery0"), new gui::Image(this, batteryOffset, 17, 0, 0, "battery1"),
        new gui::Image(this, batteryOffset, 17, 0, 0, "battery2"), new gui::Image(this, batteryOffset, 17, 0, 0, "battery3"),
        new gui::Image(this, batteryOffset, 17, 0, 0, "battery4"), new gui::Image(this, batteryOffset, 17, 0, 0, "battery5"),
    };
    batteryShowBars(0);

    charging = new Label(this, batteryOffset, 17, 30, this->drawArea.h);
    charging->setFilled(false);
    charging->setBorderColor(gui::ColorNoColor);
    charging->setFont(style::header::font::title);
    charging->setText("Z");
    charging->setVisible(false);

    sim = new Label(this, signalOffset + 50, 17, 110, this->drawArea.h);
    sim->setFilled(false);
    sim->setBorderColor(gui::ColorNoColor);
    sim->setFont(style::header::font::title);

    //icon of the lock
	lock = new gui::Image( this, 240-11,17,0,0, "lock" );

	//time label
	timeLabel = new Label(this, 0, 0, 480, this->drawArea.h );
	timeLabel->setFilled( false );
	timeLabel->setBorderColor( gui::ColorNoColor );
	timeLabel->setFont(style::header::font::time);
	timeLabel->setText("00:00");
	timeLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
}

void TopBar::setActive(std::list<std::pair<TopBar::Elements, bool>> elements)
{
    for (auto el : elements)
    {
        setActive(el.first, el.second);
    }
}

void TopBar::setActive( TopBar::Elements element, bool active ) {
	switch( element ) {
		case Elements::BATTERY: {
            elements.battery = active;
            if (Store::Battery::get().state == Store::Battery::State::Discharging)
            {
                setBatteryLevel(active ? Store::Battery::get().level : 0);
            }
            else
            {
                if (active)
                {
                    setBatteryCharging(true);
                }
                else
                {
                    charging->setVisible(false);
                    setBatteryLevel(0);
                }
            }
        } break;
		case Elements::LOCK: {
            elements.lock = active;
            lock->setVisible(active);
			if( active )
				timeLabel->setVisible(false);
		} break;
		case Elements::SIGNAL: {
            elements.signal = active;
            for (uint32_t i = 0; i < signalImgCount; ++i)
                signal[i]->setVisible(false);
			if( active )
				signal[signalStrength]->setVisible(true);
		} break;
		case Elements::TIME: {
            elements.time = active;
            timeLabel->setVisible(active);
			if( active )
				lock->setVisible(false);
		} break;
        case Elements::SIM:
            elements.sim = active;
            simSet();
            break;
    };
}

uint32_t calculateBatteryLavel(uint32_t percentage)
{
    uint32_t level = 0;
    if (percentage <= 5)
        level = 0;
    else if (percentage <= 27)
        level = 1;
    else if (percentage <= 50)
        level = 2;
    else if (percentage <= 73)
        level = 3;
    else if (percentage <= 95)
        level = 4;
    else
        level = 5;

    if (level >= batteryLevelCount)
    {
        LOG_ERROR("Battery level calculations are done wrong!");
        return batteryLevelCount - 1;
    }
    return level;
}

bool TopBar::setBatteryLevel(uint32_t percent)
{
    if (Store::Battery::get().state != Store::Battery::State::Discharging)
    {
        return false;
    }
    charging->setVisible(false);
    batteryShowBars(calculateBatteryLavel(percent));
    return true;
}

void TopBar::setBatteryCharging(bool plugged)
{
    if (plugged)
    {
        batteryShowBars(0);
    }
    if (charging == nullptr)
        return;
    if (plugged)
    {
        charging->setVisible(true);
        batteryShowBars(0);
    }
    else
    {
        charging->setVisible(false);
        setBatteryLevel(Store::Battery::get().level);
    }
}

void TopBar::setSignalStrength( uint32_t sth) {

	signalStrength = sth;
	for( uint32_t i=0; i<signalImgCount; i++ ) {
		signal[i]->setVisible( false );
	}
	signal[signalStrength]->setVisible( true );
};

void TopBar::setTime( const UTF8& time ) {
	timeLabel->setText(time);
}

void TopBar::setTime( const uint32_t& time, bool mode24H ) {

	std::time_t t= time;
	setTime(utils::time::SysTime());

	timeMode = (mode24H?TimeMode::TIME_24H:TimeMode::TIME_12H);
	this->time = time;
}

UTF8 TopBar::getTimeString() {
	setTime( time, (timeMode == TimeMode::TIME_24H )?true:false );
	return timeLabel->getText();
}

std::list<DrawCommand*> TopBar::buildDrawList() {

	//make sure that time text is updated.
	setTime( time, (timeMode == TimeMode::TIME_24H )?true:false );

	return Rect::buildDrawList();
}

void TopBar::simSet()
{
    if (!sim)
    {
        return;
    }
    else
    {
        if (elements.sim)
        {
            switch (Store::GSM::get()->sim)
            {
            case Store::GSM::SIM::NONE:
                sim->setText("x");
                break;
            case Store::GSM::SIM::SIM1:
                sim->setText("1");
                break;
            case Store::GSM::SIM::SIM2:
                sim->setText("2");
                break;
            }
        }
        else
        {
            sim->visible = false;
        }
    }
}

} /* namespace gui */
