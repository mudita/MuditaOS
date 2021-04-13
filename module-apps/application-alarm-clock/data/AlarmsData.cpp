// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsData.hpp"
#include <map>

static const std::map<WeekDayIso, const char *> weekDaysAbbreviation = {{WeekDayIso::Monday, "common_mon"},
                                                                        {WeekDayIso::Tuesday, "common_tue"},
                                                                        {WeekDayIso::Wednesday, "common_wed"},
                                                                        {WeekDayIso::Thursday, "common_thu"},
                                                                        {WeekDayIso::Friday, "common_fri"},
                                                                        {WeekDayIso::Saturday, "common_sat"},
                                                                        {WeekDayIso::Sunday, "common_sun"}};

CustomRepeatValueParser::CustomRepeatValueParser(uint32_t repeatValue)
{
    OptionParser parser;
    weekDayData = parser.setWeekDayOptions(repeatValue, std::make_unique<WeekDaysRepeatData>());
}

std::string CustomRepeatValueParser::getWeekDaysText() const
{
    std::string weekDaysText;
    for (auto const &[key, val] : weekDaysAbbreviation) {
        if (weekDayData->getData(static_cast<uint32_t>(key))) {
            weekDaysText += utils::translate(val) + ", ";
        }
    }
    if (!weekDaysText.empty()) {
        weekDaysText.erase(weekDaysText.end() - 2);
    }
    return weekDaysText;
}

bool CustomRepeatValueParser::isCustomValueWeekDays() const
{
    return weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Monday)) &&
           weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Tuesday)) &&
           weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Wednesday)) &&
           weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Thursday)) &&
           weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Friday));
}

bool CustomRepeatValueParser::isCustomValueEveryday() const
{
    return isCustomValueWeekDays() && weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Saturday)) &&
           weekDayData->getData(static_cast<uint32_t>(WeekDayIso::Sunday));
}
