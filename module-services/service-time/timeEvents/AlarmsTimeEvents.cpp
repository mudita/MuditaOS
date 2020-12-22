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
        auto selectTurnedOn = dynamic_cast<db::query::alarms::SelectTurnedOnResult *>(nextEventQueryResult.get());
        if (selectTurnedOn == nullptr) {
            return 0;
        }
        std::vector<AlarmsRecord> records = selectTurnedOn->getResult();
        if (records.empty()) {
            return 0;
        }

        auto actualTimePoint = TimePointNow();
        auto weekDay         = WeekdayIndexFromTimePoint(actualTimePoint);
        std::vector<AlarmsRecord> nearestAlarms;
        for (uint32_t i = 0; i < date::Sunday.iso_encoding() + 1; i++) {
            for (auto &record : records) {
                auto hoursAndMinutes = TimePointToHourMinSec(record.time);
                record.time = TimePointFromYearMonthDay(TimePointToYearMonthDay(TimePointNow())) + date::days{i} +
                              hoursAndMinutes.hours() + hoursAndMinutes.minutes();
                auto buffer = record.time +
                              (static_cast<uint32_t>(record.status) - 1) * std::chrono::minutes(record.snooze) +
                              std::chrono::minutes(record.delay);
                if (buffer < TimePointNow()) {
                    continue;
                }
                if (record.repeat == static_cast<uint32_t>(AlarmRepeat::never) ||
                    record.repeat == static_cast<uint32_t>(AlarmRepeat::everyday) ||
                    (weekDay < date::Saturday.iso_encoding() - 1 &&
                     record.repeat == static_cast<uint32_t>(AlarmRepeat::weekDays))) {
                    nearestAlarms.push_back(record);
                }
                else if (weekDay == date::Saturday.iso_encoding() - 1 &&
                         record.repeat == static_cast<uint32_t>(AlarmRepeat::weekDays) &&
                         record.status > AlarmStatus::On) {
                    if (isAlarmGoingToJumpToNextDay(record)) {
                        nearestAlarms.push_back(record);
                    }
                }
                else if (record.repeat > static_cast<uint32_t>(AlarmRepeat::weekDays)) {
                    OptionParser parser;
                    auto weekDayData = parser.setWeekDayOptions(record.repeat, std::make_unique<WeekDaysRepeatData>());
                    if (weekDayData->getData(weekDay)) {
                        nearestAlarms.push_back(record);
                    }
                    uint32_t dayBefore = weekDay - 1;
                    if (dayBefore > date::Sunday.iso_encoding() - 1) {
                        dayBefore = date::Sunday.iso_encoding() - 1;
                    }
                    if (weekDayData->getData(dayBefore)) {
                        if (isAlarmGoingToJumpToNextDay(record)) {
                            nearestAlarms.push_back(record);
                        }
                    }
                }
            }
            // search in another day if there is not alarm for today
            if (nearestAlarms.empty()) {
                weekDay++;
                if (weekDay > date::Sunday.iso_encoding() - 1) {
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

        // Apply snooze and delay
        for (auto &alarm : nearestAlarms) {
            if (alarm.status > AlarmStatus::On) {
                // if alarm time after applying snoozes is going to jump to the next day
                // it means that this is alarm from yesterday and to properly handle it
                // you have to subtract one day so it will rang today not tomorrow
                if (isAlarmGoingToJumpToNextDay(alarm)) {
                    alarm.time -= date::days{1};
                }
                alarm.time = alarm.time +
                             (static_cast<uint32_t>(alarm.status) - 1) * std::chrono::minutes(alarm.snooze) +
                             std::chrono::minutes(alarm.delay);
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

        auto alarmTime = nearestAlarms.at(0).time;
        nearestAlarms.erase(std::remove_if(nearestAlarms.begin(),
                                           nearestAlarms.end(),
                                           [alarmTime](const AlarmsRecord &rec) { return alarmTime != rec.time; }),
                            nearestAlarms.end());
        if (nearestAlarms.size() > 1) {
            LOG_DEBUG("Multiple alarms at the same time");
        }
        alarmRecords  = nearestAlarms;
        alarmsRecord = nearestAlarms.at(0);
        auto duration = alarmsRecord.time - TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }
        // restore original alarm time if snooze was applied
        if (alarmsRecord.status > AlarmStatus::On) {
            alarmsRecord.time =
                alarmsRecord.time -
                (static_cast<uint32_t>(alarmsRecord.status) - 1) * std::chrono::minutes(alarmsRecord.snooze) -
                std::chrono::minutes(alarmsRecord.delay);
        }
        LOG_DEBUG("How many miliseconds to alarm: %i",
                  static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()));
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool AlarmsTimeEvents::sendEventFiredQuery()
    {
        sendNextEventQuery();
        return true;
    }

    void AlarmsTimeEvents::invokeEvent()
    {
        // auto alarm = std::make_shared<AlarmsRecord>(alarmsRecord);
        // auto data  = std::make_unique<AlarmRecordData>(alarm);
        auto data = std::make_unique<AlarmRecordData>(alarmRecords);

        app::manager::Controller::sendAction(service(), app::manager::actions::ShowAlarm, std::move(data));
    }

    bool AlarmsTimeEvents::isAlarmGoingToJumpToNextDay(AlarmsRecord record)
    {
        auto buffer = TimePointToYearMonthDay(record.time);
        record.time = record.time + (static_cast<uint32_t>(record.status) - 1) * std::chrono::minutes(record.snooze) +
                      std::chrono::minutes(record.delay);
        return buffer.day() != TimePointToYearMonthDay(record.time).day();
    }
} // namespace stm
