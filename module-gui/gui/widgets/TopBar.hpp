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
	};
	static const uint32_t signalImgCount = 6;
	static const uint32_t batteryLevelCount = 6;
protected:
	static uint32_t signalStrength;
	static uint32_t batteryLevel;
	Image* signal[6];
	Image* lock;
	Image* battery[6];
	void prepareWidget();
public:


	TopBar();
	TopBar( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h );
	virtual ~TopBar();
	/**
	 * @brief Hides or shows images.
	 */
	void setActive( TopBar::Elements element, bool active );
	/**
	 * @brief Sets charge level of the battery. This will cause appropriate image to be displayed.
	 */
	void setBatteryLevel( uint32_t level ) { batteryLevel = level; };
	/**
	 * @brief Sets signal strength. This will cause appropriate image to be displayed.
	 */
	void setSignalStrength( uint32_t sth) { signalStrength = sth; };
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_ */
