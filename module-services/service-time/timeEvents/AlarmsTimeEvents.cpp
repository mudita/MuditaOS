// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/AlarmsTimeEvents.hpp>
#include <service-time/TimeEvents.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <module-apps/application-alarm-clock/data/AlarmsData.hpp>
#include <module-apps/application-calendar/data/OptionParser.hpp>
#include <module-db/queries/alarms/QueryAlarmsSelectTurnedOn.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>

#include <algorithm>
#include <chrono>
#include <type_traits>
#include <utility>
#include <vector>

namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    constexpr static auto eventTimerMinSkipInterval = 100ms;

    AlarmsTimeEvents::AlarmsTimeEvents(sys::Service *service) : TimeEvents(service)
    {}

    bool AlarmsTimeEvents::sendNextEventQuery()
    {
        return DBServiceAPI::GetQuery(
            service(), db::Interface::Name::Alarms, std::make_unique<db::query::alarms::SelectTurnedOn>());
    }

    uint32_t AlarmsTimeEvents::calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        auto firstUpcomingQuery = dynamic_cast<db::query::alarms::SelectTurnedOnResult *>(nextEventQueryResult.get());
        if (firstUpcomingQuery == nullptr) {
            return 0;
        }
        std::vector<AlarmsRecord> records = firstUpcomingQuery->getResult();
        if (records.empty()) {
            return 0;
        }

        auto actualTimePoint = TimePointNow();
        auto weekDay         = WeekdayIndexFromTimePoint(actualTimePoint);
        std::vector<AlarmsRecord> nearestAlarms;
        for (uint32_t i = 0; i < 7; i++) {
            for (auto &record : records) {
                auto hoursAndMinutes = TimePointToHourMinSec(record.time);
                record.time = TimePointFromYearMonthDay(TimePointToYearMonthDay(TimePointNow())) + date::days{i} +
                              hoursAndMinutes.hours() + hoursAndMinutes.minutes();
                if (record.repeat == static_cast<uint32_t>(AlarmRepeat::never) ||
                    record.repeat == static_cast<uint32_t>(AlarmRepeat::everyday)) {
                    nearestAlarms.push_back(record);
                }
                else if (weekDay < 6 && record.repeat == static_cast<uint32_t>(AlarmRepeat::weekDays)) {
                    nearestAlarms.push_back(record);
                }
                else if (weekDay == 6 && record.repeat == static_cast<uint32_t>(AlarmRepeat::weekDays) &&
                         record.status > AlarmStatus::On) {
                    auto buffer = TimePointToYearMonthDay(record.time);
                    record.time =
                        record.time + (static_cast<uint32_t>(record.status) - 1) * std::chrono::minutes(record.snooze);
                    if (buffer.day() != TimePointToYearMonthDay(record.time).day()) {
                        nearestAlarms.push_back(record);
                    }
                }
                else {
                    OptionParser parser;
                    auto weekDayData = parser.setWeekDayOptions(record.repeat, std::make_unique<WeekDaysRepeatData>());
                    if (weekDayData->getData(weekDay)) {
                        nearestAlarms.push_back(record);
                    }
                    uint32_t dayBefore = weekDay - 1;
                    if (dayBefore > 7) {
                        dayBefore = 7;
                    }
                    if (weekDayData->getData(dayBefore)) {
                        auto buffer = TimePointToYearMonthDay(record.time);
                        record.time = record.time +
                                      (static_cast<uint32_t>(record.status) - 1) * std::chrono::minutes(record.snooze);
                        if (buffer.day() != TimePointToYearMonthDay(record.time).day()) {
                            nearestAlarms.push_back(record);
                        }
                    }
                }
            }
            if (nearestAlarms.empty()) {
                weekDay++;
                if (weekDay > 6) {
                    weekDay = 0;
                }
            }
            else {
                break;
            }
        }
        if (nearestAlarms.empty()) {
            return 0;
        }

        // TODO: alarms with snoozes between days
        // Apply snooze
        for (auto &alarm : nearestAlarms) {
            if (alarm.status > AlarmStatus::On) {
                // auto buffer = TimePointToYearMonthDay(alarm.time);
                alarm.time =
                    alarm.time + (static_cast<uint32_t>(alarm.status) - 1) * std::chrono::minutes(alarm.snooze);
                // if alarm time after applying snoozes is going to jump to the next day
                // it means that this is alarm from yesterday and to properly handle it
                // you have to subtract one day so it will rang today not tomorrow
                // if (buffer.day() != TimePointToYearMonthDay(alarm.time).day()) {
                //    alarm.time -= date::days{1};
                //}
                if (alarm.time < TimePointNow()) {
                    alarm.time += date::days{1};
                }
            }
            // if alarm status is not Off and alarm time is smaller than actual time
            // it means that this alarm should ring the next day
            else if (alarm.time < TimePointNow()) {
                alarm.time += date::days{1};
            }
        }

        std::sort(nearestAlarms.begin(),
                  nearestAlarms.end(),
                  [](const AlarmsRecord &first, const AlarmsRecord &second) { return first.time < second.time; });

        alarmsRecord = nearestAlarms.at(0);
        auto duration = alarmsRecord.time - TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }
        // restore original alarm time if snooze was applied
        if (alarmsRecord.status > AlarmStatus::On) {
            alarmsRecord.time = alarmsRecord.time - (static_cast<uint32_t>(alarmsRecord.status) - 1) *
                                                        std::chrono::minutes(alarmsRecord.snooze);
        }
        LOG_DEBUG("How many miliseconds to alarm: %i",
                  static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()));
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool AlarmsTimeEvents::sendEventFiredQuery()
    {
        // mlucki
        // Todo: there is a place to set (in DB and optionally) the alarmsRecord as processed/invoked

        /*eventRecord.reminder_fired = TimePointNow();
        return DBServiceAPI::GetQuery(
            service(), db::Interface::Name::Alarms, std::make_unique<db::query::alarms::Edit>(alarmsRecord));*/
        // sendNextEventQuery();
        return true;
    }

    void AlarmsTimeEvents::invokeEvent()
    {
        auto alarm = std::make_shared<AlarmsRecord>(alarmsRecord);
        auto data  = std::make_unique<AlarmRecordData>(alarm);

        app::manager::Controller::sendAction(service(), app::manager::actions::ShowAlarm, std::move(data));
    }
} // namespace stm
