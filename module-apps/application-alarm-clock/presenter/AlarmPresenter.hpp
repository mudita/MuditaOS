// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "log.hpp"

#include <AlarmEventRecord.hpp>
#include <time/time_locale.hpp>
#include <rrule/rrule/rrule.hpp>
#include <utf8/UTF8.hpp>

#include <functional>
#include <list>
#include <memory>

namespace app::alarmClock
{
    typedef utils::time::Locale utl;

    class AlarmPresenter;

    class AlarmEventItem;

    class AlarmPresenter
    {
      private:
        /// our model
        std::shared_ptr<AlarmEventRecord> alarm;
        /// view managed

      protected:
        /// changes day from lib to our days from monday
        utl::Day dayToDay(uint32_t day_no);
        /// changes day from our days to lib format
        uint8_t dayToDay(utl::Day);
        uint8_t set_bit_days();
        static constexpr uint8_t weekdaysMask = 0b0111110;
        static constexpr uint8_t weekMask     = 0b1111111;

      public:
        enum class Spinner
        {
            Never,    /// we do not have any events set
            OnDay,    /// we have event on some - single day
            Custom,   /// there is some custom setup i.e. weekly + sunday - monday
            Weekly,   /// everyday occurrence
            Weekdays, /// every monday to friday occurrence
        };

        virtual ~AlarmPresenter() = default;
        explicit AlarmPresenter(std::shared_ptr<AlarmEventRecord> alarm) : alarm(std::move(alarm)){};

        /// get description for days in week as a coma separated string days, or weekday etc
        UTF8 getDescription();
        /// get information what time of occurrences we have in rrule
        /// please see `enum::Spinner` for more details
        Spinner getSpinner();
        /// set proper rrule from spinner
        void setSpinner(Spinner, const std::function<void(Spinner)> &cb);

        /// set values in model based on list of Enums (Mon, Tue etc)
        void setDays(const std::list<utl::Day> &days);
        /// get values in model as a list
        std::list<utl::Day> getDays();

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

    class AlarmEventItem
    {
        AlarmPresenter p;

      protected:
        auto &presenter()
        {
            return p;
        }

      public:
        explicit AlarmEventItem(AlarmPresenter p) : p(p)
        {}
    };

} // namespace app::alarmClock
