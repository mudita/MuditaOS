/*
 * @file TopBar.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_
#define MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_

#include "Rect.hpp"
#include "Label.hpp"
#include "Image.hpp"

namespace gui {

class TopBar: public Rect {
	static const uint32_t signalOffset;
	static const uint32_t batteryOffset;
public:
	enum class Elements {
		SIGNAL = 0x01,
		LOCK,
		BATTERY,
		TIME,
	};
	enum class TimeMode {
		TIME_12H,
		TIME_24H
	};
	static const uint32_t signalImgCount = 6;
	static const uint32_t batteryLevelCount = 6;
	static uint32_t time;
protected:
	static uint32_t signalStrength;
	static uint32_t batteryLevel;
	Label* timeLabel;
	Image* signal[6];
	Image* lock;
	Image* battery[6];
	void prepareWidget();
public:


	TopBar();
	TopBar( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h );
	virtual ~TopBar();


	/**
	 * @brief Sets mode of time displaying according to TimeMode enum
	 * @note Variable is common for all instances of TopBar
	 */

	/**
	 * @brief Hides or shows images.
	 * @note LOCK and TIME are located in the same place so only 1 can be active at the same time.
	 */
	void setActive( TopBar::Elements element, bool active );
	/**
	 * @brief Sets charge level of the battery. This will cause appropriate image to be displayed.
	 */
	void setBatteryLevel( uint32_t level );
	/**
	 * @brief Sets signal strength. This will cause appropriate image to be displayed.
	 */
	void setSignalStrength( uint32_t sth);
	uint32_t getBatteryLevel() {return batteryLevel; };
	uint32_t getSignalStrength() {return signalStrength; };

	void setTime( const UTF8& time );
	void setTime( const uint32_t& time, bool mode24H );
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_ */
