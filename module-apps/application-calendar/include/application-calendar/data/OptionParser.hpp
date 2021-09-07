// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarData.hpp"

class OptionParser
{
  private:
    constexpr static const int start_bit   = 16;
    constexpr static const int options_num = 7;

  public:
    OptionParser() = default;
    std::unique_ptr<WeekDaysRepeatData> setWeekDayOptions(const uint32_t &dataDB,
                                                          std::unique_ptr<WeekDaysRepeatData> weekDayOptions)
    {
        const uint32_t startBit        = start_bit;
        const uint32_t numberOfOptions = options_num;
        for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
            if (dataDB & (1 << i)) {
                weekDayOptions->setData(i - startBit);
            }
        }
        return weekDayOptions;
    }

    uint32_t getDatabaseFieldValue(std::unique_ptr<WeekDaysRepeatData> weekDayOptions)
    {
        uint32_t dataDB                = 0;
        const uint32_t startBit        = start_bit;
        const uint32_t numberOfOptions = options_num;
        for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
            if (weekDayOptions->getData(i - startBit)) {
                dataDB += (1 << i);
            }
        }
        return dataDB;
    }

    uint32_t getNoCustomValue(uint32_t dataDB)
    {
        /// Set all bits in custom field to zero
        /// DataDB input is not modifyed
        const uint32_t startBit        = start_bit;
        const uint32_t numberOfOptions = options_num;
        for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
            if (dataDB & (1 << i)) {
                dataDB -= (1 << i);
            }
        }
        return dataDB;
    }
};
