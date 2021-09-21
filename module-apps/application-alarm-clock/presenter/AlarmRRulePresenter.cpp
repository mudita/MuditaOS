// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmRRulePresenter.hpp"
#include "log/log.hpp"

namespace app::alarmClock
{
    AlarmRRulePresenter::AlarmRRulePresenter(std::shared_ptr<AlarmEventRecord> recordToLoad) : alarm(recordToLoad)
    {}

    void AlarmRRulePresenter::loadRecord(std::shared_ptr<AlarmEventRecord> recordToLoad)
    {
        alarm = std::move(recordToLoad);
    }

    utl::Day AlarmRRulePresenter::dayToDay(uint32_t day_no)
    {
        if (day_no == uint8_t(rrule::RRule::RRuleWeekday::SUNDAY_WEEKDAY)) {
            return utl::Sun;
        }
        return utl::Day(day_no - 1);
    }

    uint8_t AlarmRRulePresenter::dayToDay(utl::Day day)
    {
        if (day == utl::Sun) {
            return uint8_t(rrule::RRule::RRuleWeekday::SUNDAY_WEEKDAY);
        }
        return day + 1;
    }

    uint8_t AlarmRRulePresenter::setBitDays()
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

    UTF8 AlarmRRulePresenter::getDescription()
    {
        auto setDays = setBitDays();
        if (setDays == 0) {
            return utils::translate("app_alarm_clock_repeat_never");
        }
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

    bool AlarmRRulePresenter::isDaySet(uint8_t &days, uint8_t day)
    {
        return 0x1 & (days >> day);
    }

    AlarmRRulePresenter::RRuleOptions AlarmRRulePresenter::getOption()
    {
        auto setDays = setBitDays();
        if (setDays == 0) {
            return RRuleOptions::Never;
        }
        if (setDays == weekdaysMask) {
            return RRuleOptions::Weekdays;
        }
        if (setDays == weekMask) {
            return RRuleOptions::Everyday;
        }
        auto get_set_days_count = [&]() {
            uint8_t singleday = 0;
            for (unsigned int i = 0; i < utl::num_days; ++i) {
                singleday += 0x1 & (setDays >> i);
            }
            return singleday;
        };
        if (get_set_days_count() == 1) {
            return RRuleOptions::OnDay;
        }
        return RRuleOptions::Custom;
    }

    std::vector<std::pair<std::string, bool>> AlarmRRulePresenter::getCustomDays()
    {
        std::vector<std::pair<std::string, bool>> selectedDays;
        auto setDays = setBitDays();

        for (unsigned int i = 0; i < utl::num_days; ++i) {
            selectedDays.push_back({utils::time::Locale::get_day(i), isDaySet(setDays, i)});
        }

        return selectedDays;
    }

    void AlarmRRulePresenter::setOption(AlarmRRulePresenter::RRuleOptions options, const std::list<utl::Day> &days)
    {
        switch (options) {
        case AlarmRRulePresenter::RRuleOptions::Never:
            setDays({});
            break;
        case AlarmRRulePresenter::RRuleOptions::OnDay:
            setDays(days);
            break;
        case AlarmRRulePresenter::RRuleOptions::Custom:
            setDays(days);
            break;
        case AlarmRRulePresenter::RRuleOptions::Everyday:
            setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
            break;
        case AlarmRRulePresenter::RRuleOptions::Weekdays:
            setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
            break;
        }
    }

    void AlarmRRulePresenter::setDays(const std::list<utl::Day> &days)
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

    std::list<utl::Day> AlarmRRulePresenter::getDays()
    {
        std::list<utl::Day> ret;
        auto setDays = setBitDays();
        for (unsigned int i = 0; i < utl::num_days; ++i) {
            if ((0x1 & (setDays >> i)) != 0) {
                ret.push_back(utl::Day(i));
            }
        }
        return ret;
    }
} // namespace app::alarmClock
