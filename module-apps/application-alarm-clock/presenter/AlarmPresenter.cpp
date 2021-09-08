// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmPresenter.hpp"
#include "log.hpp"

namespace app::alarmClock
{

    utl::Day AlarmPresenter::dayToDay(uint32_t day_no)
    {
        if (day_no == uint8_t(rrule::RRule::RRuleWeekday::SUNDAY_WEEKDAY)) {
            return utl::Sun;
        }
        return utl::Day(day_no - 1);
    }

    uint8_t AlarmPresenter::dayToDay(utl::Day day)
    {
        if (day == utl::Sun) {
            return uint8_t(rrule::RRule::RRuleWeekday::SUNDAY_WEEKDAY);
        }
        return day + 1;
    }

    uint8_t AlarmPresenter::set_bit_days()
    {
        const auto &rr = rrule::RRule(alarm->rruleText);
        uint8_t days   = 0;
        if (rr.freq != rrule::RRule::RRuleFrequency::WEEKLY_RECURRENCE) {
            return 0;
        }
        for (const auto &day : rr.byDay) {
            if (auto real = dayToDay(day); real < utl::num_days) {
                days |= 1 << real;
            }
        }
        return days;
    }

    UTF8 AlarmPresenter::getDescription()
    {
        auto setDays = set_bit_days();
        if (setDays == weekdaysMask) {
            return utils::translate("app_alarm_clock_repeat_week_days");
        }
        if (setDays == weekMask) {
            return utils::translate("app_alarm_clock_repeat_everyday");
        }
        UTF8 retval = "";
        if (setDays > 0) {
            for (unsigned int i = 0; i < utl::num_days; ++i) {
                if ((0x1 & (setDays >> i)) != 0) {
                    auto dayname = utils::time::Locale::get_short_day(i);
                    retval += dayname + ",";
                }
            }
            if (retval.length() > 0) {
                retval.removeChar(retval.length() - 1);
            }
        }
        return retval;
    }

    AlarmPresenter::Spinner AlarmPresenter::getSpinner()
    {
        auto setDays = set_bit_days();
        if (setDays == 0) {
            return Spinner::Never;
        }
        if (setDays == weekdaysMask) {
            return Spinner::Weekdays;
        }
        if (setDays == weekMask) {
            return Spinner::Weekly;
        }
        auto get_set_days_count = [&]() {
            uint8_t singleday = 0;
            for (unsigned int i = 0; i < utl::num_days; ++i) {
                singleday += 0x1 & (setDays >> i);
            }
            return singleday;
        };
        if (get_set_days_count() == 1) {
            return Spinner::OnDay;
        }
        return Spinner::Custom;
    }

    void AlarmPresenter::setSpinner(AlarmPresenter::Spinner spin,
                                    const std::function<void(AlarmPresenter::Spinner)> &cb)
    {
        switch (spin) {
        case AlarmPresenter::Spinner::Never:
            setDays({});
            break;
        case AlarmPresenter::Spinner::OnDay:
            if (cb != nullptr) {
                cb(spin);
            }
            break;
        case AlarmPresenter::Spinner::Custom:
            if (cb != nullptr) {
                cb(spin);
            }
            break;
        case AlarmPresenter::Spinner::Weekly:
            setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
            break;
        case AlarmPresenter::Spinner::Weekdays:
            setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
            break;
        }
    }

    void AlarmPresenter::setDays(const std::list<utl::Day> &days)
    {
        if (days.empty()) {
            alarm->rruleText = "";
        }
        else {
            auto rr = rrule::RRule();
            // interval each week
            rr.interval = 1;
            rr.freq     = rrule::RRule::RRuleFrequency::WEEKLY_RECURRENCE;
            for (const auto &day : days) {
                rr.byDay.push_back(dayToDay(day));
            }
            alarm->rruleText = rr.parseToString();
        }
    }

    std::list<utl::Day> AlarmPresenter::getDays()
    {
        std::list<utl::Day> ret;
        auto setDays = set_bit_days();
        for (unsigned int i = 0; i < utl::num_days; ++i) {
            if ((0x1 & (setDays >> i)) != 0) {
                ret.push_back(utl::Day(i));
            }
        }
        return ret;
    }

} // namespace app::alarmClock
