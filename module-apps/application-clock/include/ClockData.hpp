// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_
#define MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_

#include <cstdint>
#include "SwitchData.hpp"

class ClockData : public gui::SwitchData
{
  public:
    uint32_t hour;
    uint32_t minute;
    ClockData(uint32_t hour = 0, uint32_t minute = 0) : hour{hour}, minute{minute} {};
    virtual ~ClockData(){};
};

#endif /* MODULE_APPS_APPLICATION_CLOCK_CLOCKDATA_HPP_ */
