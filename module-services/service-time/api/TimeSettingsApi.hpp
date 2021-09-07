// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/time_locale.hpp>

namespace stm::api
{
    /**
     * Gets value corresponded to current Automatic Date and Time setting stored in DB
     * @return actual setting value
     */
    bool isAutomaticDateAndTime();
    /**
     * Gets value corresponded to current Time format stored in DB
     * @return actual setting value
     */
    utils::time::Locale::DateFormat dateFormat();
    /**
     * Gets value corresponded to current Date format setting stored in DB
     * @return actual setting value
     */
    utils::time::Locale::TimeFormat timeFormat();
    /**
     *It checks if there is 12h time format set
     * @return true when 12h format is set, false when 24h format is set
     */
    bool isTimeFormat12h();
    /**
     * Gets value corresponded to current timezone setting stored in DB
     * @return current timezone name
     */
    const std::string getCurrentTimezoneName();
    /**
     * Gets value corresponded to current timezone setting stored in DB
     * @return current timezone rules
     */
    const std::string getCurrentTimezoneRules();
} // namespace stm::api
