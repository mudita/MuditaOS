/*
 * @file ClockData.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 18 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_
#define MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_

#include <cstdint>
#include "SwitchData.hpp"

class ClockData : public gui::SwitchData {
public:
	uint32_t hour;
	uint32_t minute;
	ClockData ( uint32_t hour = 0, uint32_t minute = 0) : hour{hour}, minute{minute} {};
	virtual ~ClockData () {};
};



#endif /* MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_ */
