// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RTCCommandInterface.hpp"

#include <ctime>
#include <chrono>

class TimeManager
{

  public:
    /**
     * @brief Basic time, date and timezone manager.
     * @param rtcCommandInterface bsp rtc related commands
     */
    explicit TimeManager(std::unique_ptr<RTCCommandInterface> rtcCommandInterface)
        : rtcCommand(std::move(rtcCommandInterface))
    {}
    /**
     * Handles GSM time update notification. Sets custom timezone.
     * @param time new UTC time
     * @param timezoneOffset timezone offset related to UTC time
     */
    void handleCellularTimeUpdate(const struct tm time, std::chrono::minutes timezoneOffset);
    /**
     * Handles time change request.
     * @param time UTC time to set
     */
    void handleTimeChangeRequest(const time_t &time);

  private:
    std::unique_ptr<RTCCommandInterface> rtcCommand;
};
