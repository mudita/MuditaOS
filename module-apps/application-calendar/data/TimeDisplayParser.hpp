// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "module-apps/application-calendar/widgets/CalendarStyle.hpp"
#include <Utils.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <time/time_locale.hpp>

class TimeDisplayParser
{

  private:
    std::string getAMorPM(bool is_am)
    {
        if (is_am) {
            return utils::localize.get(utils::time::Locale::getAM());
        }
        else {
            return utils::localize.get(utils::time::Locale::getPM());
        }
    }

  public:
    TimeDisplayParser() = default;

    std::string getTimeString(const std::shared_ptr<EventsRecord> &event,
                              bool isShortVersion = false,
                              bool isMode24H      = false)
    {
        auto start_time = TimePointToHourMinSec(event->date_from);
        auto end_time   = TimePointToHourMinSec(event->date_till);

        if (start_time.hours().count() == 0 && start_time.minutes().count() == 0 &&
            end_time.hours().count() == style::window::calendar::time::max_hour_24H_mode &&
            end_time.minutes().count() == style::window::calendar::time::max_minutes) {
            return utils::localize.get("app_calendar_all_day");
        }
        else {
            bool start_is_am = true;
            bool end_is_am   = true;
            auto start_h     = std::to_string(start_time.hours().count());
            auto start_min   = std::to_string(start_time.minutes().count());
            auto end_h       = std::to_string(end_time.hours().count());
            auto end_min     = std::to_string(end_time.minutes().count());

            if (start_min.length() < style::window::calendar::time::max_time_length) {
                start_min.insert(0, style::window::calendar::time::max_time_length / 2, '0');
            }
            if (end_min.length() < style::window::calendar::time::max_time_length) {
                end_min.insert(0, style::window::calendar::time::max_time_length / 2, '0');
            }

            if (!isMode24H) {
                start_is_am = date::is_am(start_time.hours());
                end_is_am   = date::is_am(end_time.hours());
                start_h     = std::to_string(date::make12(start_time.hours()).count());
                end_h       = std::to_string(date::make12(end_time.hours()).count());
            }

            if (start_h.length() < style::window::calendar::time::max_time_length) {
                start_h.insert(0, style::window::calendar::time::max_time_length / 2, '0');
            }
            if (end_h.length() < style::window::calendar::time::max_time_length) {
                end_h.insert(0, style::window::calendar::time::max_time_length / 2, '0');
            }

            if (isShortVersion) {
                if (!isMode24H) {
                    return TimePointToHourString12H(event->date_from) + ":" +
                           TimePointToMinutesString(event->date_from) + " " + getAMorPM(start_is_am);
                }
                else {
                    return TimePointToHourString24H(event->date_from) + ":" +
                           TimePointToMinutesString(event->date_from);
                }
            }
            else {
                if (!isMode24H) {
                    if (start_is_am != end_is_am) {
                        return TimePointToHourString12H(event->date_from) + ":" +
                               TimePointToMinutesString(event->date_from) + " " + getAMorPM(start_is_am) + " - " +
                               TimePointToHourString12H(event->date_till) + ":" +
                               TimePointToMinutesString(event->date_till) + " " + getAMorPM(end_is_am);
                    }
                    else {
                        return TimePointToHourString12H(event->date_from) + ":" +
                               TimePointToMinutesString(event->date_from) + " - " +
                               TimePointToHourString12H(event->date_till) + ":" +
                               TimePointToMinutesString(event->date_till) + " " + getAMorPM(start_is_am);
                    }
                }
                else {
                    return TimePointToHourString24H(event->date_from) + ":" +
                           TimePointToMinutesString(event->date_from) + " - " +
                           TimePointToMinutesString(event->date_till) + ":" +
                           TimePointToMinutesString(event->date_till);
                }
            }
        }
    }
};
