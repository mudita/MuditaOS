// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "log/log.hpp"

#include <AlarmEventRecord.hpp>
#include <time/time_locale.hpp>
#include <rrule/rrule.hpp>
#include <utf8/UTF8.hpp>

#include <functional>
#include <list>
#include <memory>

namespace app::alarmClock
{
    typedef utils::time::Locale utl;

    class AlarmRRulePresenter;

    class AlarmRRuleItem;

    class AlarmRRulePresenter
    {
      private:
        /// our model
        std::shared_ptr<AlarmEventRecord> alarm;
        /// view managed

        /// check if day is set
        [[nodiscard]] bool isDaySet(uint8_t &days, uint8_t day);

      protected:
        /// changes day from lib to our days from monday
        utl::Day dayToDay(uint32_t day_no);
        /// changes day from our days to lib format
        uint8_t dayToDay(utl::Day);
        uint8_t setBitDays();
        static constexpr uint8_t weekdaysMask = 0b0111110;
        static constexpr uint8_t weekMask     = 0b1111111;

      public:
        enum class RRuleOptions
        {
            Never,    /// we do not have any events set
            OnDay,    /// we have event on some - single day
            Custom,   /// there is some custom setup i.e. weekly + sunday - monday
            Everyday, /// everyday occurrence
            Weekdays, /// every monday to friday occurrence
        };

        virtual ~AlarmRRulePresenter() = default;
        explicit AlarmRRulePresenter(){};
        explicit AlarmRRulePresenter(std::shared_ptr<AlarmEventRecord> recordToLoad);

        void loadRecord(std::shared_ptr<AlarmEventRecord> recordToLoad);

        /// get description for days in week as a coma separated string days, or weekday etc
        UTF8 getDescription();
        /// get information what time of occurrences we have in rrule
        /// please see `enum::RRuleOptions` for more details
        RRuleOptions getOption();
        /// set proper rrule from Options spinner
        void setOption(RRuleOptions, const std::list<utl::Day> &days = {});

        /// set values in model based on list of Enums (Mon, Tue etc)
        void setDays(const std::list<utl::Day> &days);
        /// get values in model as a list
        std::list<utl::Day> getDays();
        /// get custom days vector with selected days
        std::vector<std::pair<std::string, bool>> getCustomDays();

        /// if rrule is empty we can safely assume that there is no recurrence in the event
        bool hasRecurrence() const
        {
            return alarm && !alarm->rruleText.empty();
        }

        const auto &getAlarm()
        {
            return alarm;
        }
    };

    class AlarmRRuleItem
    {
        std::shared_ptr<app::alarmClock::AlarmRRulePresenter> presenter;

      protected:
        auto &getPresenter()
        {
            return presenter;
        }

      public:
        explicit AlarmRRuleItem(std::shared_ptr<app::alarmClock::AlarmRRulePresenter> presenter)
            : presenter(std::move(presenter))
        {}
    };

} // namespace app::alarmClock
