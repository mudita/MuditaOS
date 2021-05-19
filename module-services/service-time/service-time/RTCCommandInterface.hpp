// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Service.hpp>

#include <time.h>

/**
 * @brief Basic interface for bsp rtc related commands
 */
class RTCCommandInterface
{
  public:
    /**
     * Default destructor
     */
    virtual ~RTCCommandInterface() = default;
    /**
     * Sets RTC counter
     * @param time UTC time value to sed
     */
    virtual void setTime(const struct tm &time) = 0;
    /**
     * Sets timezone variable
     * @param timezone new timezone variable
     */
    virtual void setTimezone(const std::string &timezone) = 0;
};
