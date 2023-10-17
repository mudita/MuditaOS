// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rrule.hpp"

#include <time/dateCommon.hpp>

extern "C"
{
#include <icalrecur.h>
}

namespace rrule
{
    icalrecurrencetype RRuletoIcalRecurrenceType(const RRule &rrule);

    RRule::RRuleFrequency icalFrequencyToRRuleFrequency(const icalrecurrencetype_frequency freq);
    RRule::RRuleWeekday icalWeekStartToRRuleWeekStart(const icalrecurrencetype_weekday weekday);
    icalrecurrencetype_frequency RRuleFrequencyToIcalFrequency(const RRule::RRuleFrequency freq);
    icalrecurrencetype_weekday RRuleWeekStartToIcalWeekStart(const RRule::RRuleWeekday weekday);

    template <typename arrayT, typename vectT>
    std::vector<vectT> icalArrayToVector(const arrayT *array, unsigned max_size);
    template <typename arrayT, typename vectT>
    unsigned vectorToIcalArray(const std::vector<vectT> &vect, arrayT *array, unsigned max_size);

    RRuleIter::RRuleIter(const RRule &rrule, const TimePoint eventStart)
    {
        auto icalEventStart = icaltime_from_timet_with_zone(std::chrono::system_clock::to_time_t(eventStart), 0, NULL);
        iter                = icalrecur_iterator_new(RRuletoIcalRecurrenceType(rrule), icalEventStart);
    }

    RRuleIter::~RRuleIter()
    {
        icalrecur_iterator_free(iter);
        iter = nullptr;
    }

    void RRuleIter::setRange(const TimePoint rangeStart, const TimePoint rangeEnd)
    {
        auto icalTimeStart = icaltime_from_timet_with_zone(std::chrono::system_clock::to_time_t(rangeStart), 0, NULL);
        auto icalTimeEnd   = icaltime_from_timet_with_zone(std::chrono::system_clock::to_time_t(rangeEnd), 0, NULL);

        icalrecur_iterator_set_range(iter, icalTimeStart, icalTimeEnd);
    }

    TimePoint RRuleIter::next()
    {
        icaltimetype nextIcal = icalrecur_iterator_next(iter);
        if (icaltime_is_null_time(nextIcal)) {
            return TIME_POINT_INVALID;
        }
        else {
            return std::chrono::system_clock::from_time_t(icaltime_as_timet(nextIcal));
        }
    }

    RRule::RRule(std::string_view str)
    {
        parseFromString(str);
    }

    void RRule::parseFromString(std::string_view str)
    {
        const auto icalRrule    = icalrecurrencetype_from_string(str.data());
        const time_t untilTimeT = icaltime_as_timet(icalRrule.until);

        if (untilTimeT == 0) {
            until = TIME_POINT_INVALID;
        }
        else {
            until = std::chrono::system_clock::from_time_t(untilTimeT);
        }
        freq      = icalFrequencyToRRuleFrequency(icalRrule.freq);
        count     = icalRrule.count;
        interval  = icalRrule.interval;
        weekStart = icalWeekStartToRRuleWeekStart(icalRrule.week_start);

        bySecond = icalArrayToVector<short, decltype(bySecond)::value_type>(icalRrule.by_second, ICAL_BY_SECOND_SIZE);
        byMinute = icalArrayToVector<short, decltype(byMinute)::value_type>(icalRrule.by_minute, ICAL_BY_MINUTE_SIZE);
        byHour   = icalArrayToVector<short, decltype(byHour)::value_type>(icalRrule.by_hour, ICAL_BY_HOUR_SIZE);
        byDay    = icalArrayToVector<short, decltype(byDay)::value_type>(icalRrule.by_day, ICAL_BY_DAY_SIZE);
        byMonthDay =
            icalArrayToVector<short, decltype(byMonthDay)::value_type>(icalRrule.by_month_day, ICAL_BY_MONTHDAY_SIZE);
        byYearDay =
            icalArrayToVector<short, decltype(byYearDay)::value_type>(icalRrule.by_year_day, ICAL_BY_YEARDAY_SIZE);
        byWeekNo = icalArrayToVector<short, decltype(byWeekNo)::value_type>(icalRrule.by_week_no, ICAL_BY_WEEKNO_SIZE);
        byMonth  = icalArrayToVector<short, decltype(byMonth)::value_type>(icalRrule.by_month, ICAL_BY_MONTH_SIZE);
        bySetPos = icalArrayToVector<short, decltype(bySetPos)::value_type>(icalRrule.by_set_pos, ICAL_BY_SETPOS_SIZE);
    }

    std::string RRule::parseToString()
    {
        auto icalRRule = RRuletoIcalRecurrenceType(*this);
        return std::string{icalrecurrencetype_as_string(&icalRRule)};
    }

    std::vector<TimePoint> RRule::generateEventTimePoints(const TimePoint eventStart,
                                                          const TimePoint rangeStart,
                                                          const TimePoint rangeEnd,
                                                          const unsigned int count)
    {
        std::vector<TimePoint> eventsTimePoints;
        unsigned int counter = 0;

        TimePoint singleEventTimePoint;
        auto rruleIter = RRuleIter(*this, eventStart);
        rruleIter.setRange(eventStart, rangeEnd);

        do {
            singleEventTimePoint = rruleIter.next();
            if (singleEventTimePoint < rangeStart) {
                continue;
            }
            eventsTimePoints.push_back(singleEventTimePoint);
            counter++;
        } while (singleEventTimePoint != TIME_POINT_INVALID && singleEventTimePoint < rangeEnd && counter < count);

        return eventsTimePoints;
    }

    TimePoint RRule::generateNextTimePoint(const TimePoint eventStart, const TimePoint rangeStart)
    {
        TimePoint singleEventTimePoint;
        auto rruleIter = RRuleIter(*this, eventStart);

        do {
            singleEventTimePoint = rruleIter.next();

        } while (singleEventTimePoint != TIME_POINT_INVALID && singleEventTimePoint <= rangeStart);

        return singleEventTimePoint;
    }

    TimePoint RRule::generateLastTimePoint(const TimePoint eventStart)
    {
        if (until == TIME_POINT_INVALID && count == 0) {
            return TIME_POINT_MAX;
        }

        auto lastValidEventTimePoint = TIME_POINT_MAX;
        auto singleEventTimePoint    = TIME_POINT_MAX;
        auto rruleIter               = RRuleIter(*this, eventStart);
        do {
            lastValidEventTimePoint = singleEventTimePoint;
            singleEventTimePoint    = rruleIter.next();

        } while (singleEventTimePoint != TIME_POINT_INVALID);

        return lastValidEventTimePoint;
    }

    icalrecurrencetype RRuletoIcalRecurrenceType(const RRule &rrule)
    {
        icalrecurrencetype icalRrule = ICALRECURRENCETYPE_INITIALIZER;

        icalRrule.freq  = RRuleFrequencyToIcalFrequency(rrule.freq);
        icalRrule.count = rrule.count;

        if (rrule.until != TIME_POINT_INVALID) {
            icalRrule.until = icaltime_from_timet_with_zone(std::chrono::system_clock::to_time_t(rrule.until), 0, NULL);
        }
        icalRrule.interval   = rrule.interval;
        icalRrule.week_start = RRuleWeekStartToIcalWeekStart(rrule.weekStart);

        vectorToIcalArray<short, decltype(rrule.bySecond)::value_type>(
            rrule.bySecond, icalRrule.by_second, ICAL_BY_SECOND_SIZE);
        vectorToIcalArray<short, decltype(rrule.byMinute)::value_type>(
            rrule.byMinute, icalRrule.by_minute, ICAL_BY_MINUTE_SIZE);
        vectorToIcalArray<short, decltype(rrule.byHour)::value_type>(
            rrule.byHour, icalRrule.by_hour, ICAL_BY_HOUR_SIZE);
        vectorToIcalArray<short, decltype(rrule.byDay)::value_type>(rrule.byDay, icalRrule.by_day, ICAL_BY_DAY_SIZE);

        vectorToIcalArray<short, decltype(rrule.byMonthDay)::value_type>(
            rrule.byMonthDay, icalRrule.by_month_day, ICAL_BY_MONTHDAY_SIZE);
        vectorToIcalArray<short, decltype(rrule.byYearDay)::value_type>(
            rrule.byYearDay, icalRrule.by_year_day, ICAL_BY_YEARDAY_SIZE);
        vectorToIcalArray<short, decltype(rrule.byWeekNo)::value_type>(
            rrule.byWeekNo, icalRrule.by_week_no, ICAL_BY_WEEKNO_SIZE);
        vectorToIcalArray<short, decltype(rrule.byMonth)::value_type>(
            rrule.byMonth, icalRrule.by_month, ICAL_BY_MONTH_SIZE);
        vectorToIcalArray<short, decltype(rrule.bySetPos)::value_type>(
            rrule.bySetPos, icalRrule.by_set_pos, ICAL_BY_SETPOS_SIZE);

        return icalRrule;
    }

    RRule::RRuleFrequency icalFrequencyToRRuleFrequency(const icalrecurrencetype_frequency freq)
    {
        switch (freq) {
        case ICAL_SECONDLY_RECURRENCE:
            return RRule::RRuleFrequency::SECONDLY_RECURRENCE;
        case ICAL_MINUTELY_RECURRENCE:
            return RRule::RRuleFrequency::MINUTELY_RECURRENCE;
        case ICAL_HOURLY_RECURRENCE:
            return RRule::RRuleFrequency::HOURLY_RECURRENCE;
        case ICAL_DAILY_RECURRENCE:
            return RRule::RRuleFrequency::DAILY_RECURRENCE;
        case ICAL_WEEKLY_RECURRENCE:
            return RRule::RRuleFrequency::WEEKLY_RECURRENCE;
        case ICAL_MONTHLY_RECURRENCE:
            return RRule::RRuleFrequency::MONTHLY_RECURRENCE;
        case ICAL_YEARLY_RECURRENCE:
            return RRule::RRuleFrequency::YEARLY_RECURRENCE;
        case ICAL_NO_RECURRENCE:
        default:
            return RRule::RRuleFrequency::NO_RECURRENCE;
        }
    }

    RRule::RRuleWeekday icalWeekStartToRRuleWeekStart(const icalrecurrencetype_weekday weekday)
    {
        switch (weekday) {
        case ICAL_SUNDAY_WEEKDAY:
            return RRule::RRuleWeekday::SUNDAY_WEEKDAY;
        case ICAL_MONDAY_WEEKDAY:
            return RRule::RRuleWeekday::MONDAY_WEEKDAY;
        case ICAL_TUESDAY_WEEKDAY:
            return RRule::RRuleWeekday::TUESDAY_WEEKDAY;
        case ICAL_WEDNESDAY_WEEKDAY:
            return RRule::RRuleWeekday::WEDNESDAY_WEEKDAY;
        case ICAL_THURSDAY_WEEKDAY:
            return RRule::RRuleWeekday::THURSDAY_WEEKDAY;
        case ICAL_FRIDAY_WEEKDAY:
            return RRule::RRuleWeekday::FRIDAY_WEEKDAY;
        case ICAL_SATURDAY_WEEKDAY:
            return RRule::RRuleWeekday::SATURDAY_WEEKDAY;
        case ICAL_NO_WEEKDAY:
        default:
            return RRule::RRuleWeekday::NO_WEEKDAY;
        }
    }

    icalrecurrencetype_frequency RRuleFrequencyToIcalFrequency(const RRule::RRuleFrequency freq)
    {
        switch (freq) {
        case RRule::RRuleFrequency::SECONDLY_RECURRENCE:
            return ICAL_SECONDLY_RECURRENCE;
        case RRule::RRuleFrequency::MINUTELY_RECURRENCE:
            return ICAL_MINUTELY_RECURRENCE;
        case RRule::RRuleFrequency::HOURLY_RECURRENCE:
            return ICAL_HOURLY_RECURRENCE;
        case RRule::RRuleFrequency::DAILY_RECURRENCE:
            return ICAL_DAILY_RECURRENCE;
        case RRule::RRuleFrequency::WEEKLY_RECURRENCE:
            return ICAL_WEEKLY_RECURRENCE;
        case RRule::RRuleFrequency::MONTHLY_RECURRENCE:
            return ICAL_MONTHLY_RECURRENCE;
        case RRule::RRuleFrequency::YEARLY_RECURRENCE:
            return ICAL_YEARLY_RECURRENCE;
        case RRule::RRuleFrequency::NO_RECURRENCE:
        default:
            return ICAL_NO_RECURRENCE;
        }
    }

    icalrecurrencetype_weekday RRuleWeekStartToIcalWeekStart(const RRule::RRuleWeekday weekday)
    {
        switch (weekday) {
        case RRule::RRuleWeekday::SUNDAY_WEEKDAY:
            return ICAL_SUNDAY_WEEKDAY;
        case RRule::RRuleWeekday::MONDAY_WEEKDAY:
            return ICAL_MONDAY_WEEKDAY;
        case RRule::RRuleWeekday::TUESDAY_WEEKDAY:
            return ICAL_TUESDAY_WEEKDAY;
        case RRule::RRuleWeekday::WEDNESDAY_WEEKDAY:
            return ICAL_WEDNESDAY_WEEKDAY;
        case RRule::RRuleWeekday::THURSDAY_WEEKDAY:
            return ICAL_THURSDAY_WEEKDAY;
        case RRule::RRuleWeekday::FRIDAY_WEEKDAY:
            return ICAL_FRIDAY_WEEKDAY;
        case RRule::RRuleWeekday::SATURDAY_WEEKDAY:
            return ICAL_SATURDAY_WEEKDAY;
        case RRule::RRuleWeekday::NO_WEEKDAY:
        default:
            return ICAL_NO_WEEKDAY;
        }
    }

    template <typename arrayT, typename vectT>
    std::vector<vectT> icalArrayToVector(const arrayT *array, unsigned max_size)
    {
        std::vector<vectT> vect;
        for (unsigned i = 0; (i < max_size) && (array[i] != ICAL_RECURRENCE_ARRAY_MAX); i++) {
            vect.push_back(array[i]);
        }
        return vect;
    }

    template <typename arrayT, typename vectT>
    unsigned vectorToIcalArray(const std::vector<vectT> &vect, arrayT *array, unsigned max_size)
    {
        if (vect.size() > max_size) {
            return 0;
        }

        unsigned i;
        for (i = 0; (i < vect.size()) && (i < max_size); i++) {
            array[i] = vect[i];
        }
        std::fill(array + vect.size(), array + max_size - vect.size(), ICAL_RECURRENCE_ARRAY_MAX);
        return i;
    }
} // namespace rrule
