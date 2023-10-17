// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/dateCommon.hpp>

#include <cstdint>
#include <string>
#include <vector>

struct icalrecur_iterator_impl;
typedef struct icalrecur_iterator_impl icalrecur_iterator;

namespace rrule
{
    class RRule
    {

      public:
        enum class RRuleFrequency
        {
            SECONDLY_RECURRENCE,
            MINUTELY_RECURRENCE,
            HOURLY_RECURRENCE,
            DAILY_RECURRENCE,
            WEEKLY_RECURRENCE,
            MONTHLY_RECURRENCE,
            YEARLY_RECURRENCE,
            NO_RECURRENCE,
        };

        /// Information: by definition weekday = monday - friday, therefore our weekday is not a proper weekday
        enum class RRuleWeekday
        {
            NO_WEEKDAY,
            SUNDAY_WEEKDAY,
            MONDAY_WEEKDAY,
            TUESDAY_WEEKDAY,
            WEDNESDAY_WEEKDAY,
            THURSDAY_WEEKDAY,
            FRIDAY_WEEKDAY,
            SATURDAY_WEEKDAY,
        };

        RRuleFrequency freq{RRuleFrequency::NO_RECURRENCE};
        /* until and count are mutually exclusive. */
        std::uint32_t count{0};
        TimePoint until{TIME_POINT_INVALID};
        std::uint16_t interval{0};
        RRuleWeekday weekStart{RRuleWeekday::NO_WEEKDAY};

        std::vector<std::uint8_t> bySecond;
        std::vector<std::uint8_t> byMinute;
        std::vector<std::uint8_t> byHour;
        std::vector<std::uint8_t> byDay;
        std::vector<std::uint8_t> byMonthDay;
        std::vector<std::uint16_t> byYearDay;
        std::vector<std::uint8_t> byWeekNo;
        std::vector<std::uint8_t> byMonth;
        std::vector<std::uint16_t> bySetPos;

        RRule() = default;
        explicit RRule(std::string_view str);

        void parseFromString(std::string_view str);
        std::string parseToString();
        std::vector<TimePoint> generateEventTimePoints(const TimePoint eventStart,
                                                       const TimePoint rangeStart,
                                                       const TimePoint rangeEnd,
                                                       const unsigned int count);
        TimePoint generateNextTimePoint(const TimePoint eventStart, const TimePoint rangeStart);
        TimePoint generateLastTimePoint(const TimePoint eventStart);
    };

    class RRuleIter
    {

        icalrecur_iterator *iter;

      public:
        RRuleIter(const RRule &rrule, const TimePoint eventStart);
        ~RRuleIter();

        void setRange(const TimePoint rangeStart, const TimePoint rangeEnd);
        TimePoint next();
    };
} // namespace rrule
