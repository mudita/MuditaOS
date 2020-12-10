// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/AlarmsTimeEvents.hpp>
#include <service-time/TimeEvents.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <module-apps/application-alarm-clock/data/AlarmsData.hpp>
#include <module-apps/application-calendar/data/OptionParser.hpp>
#include <module-db/queries/alarms/QueryAlarmsSelectTurnedOn.hpp>
#include <module-gui/gui/SwitchData.hpp>
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
        TimePoint filterFrom = TimePointNow();
        TimePoint filterTill = filterFrom;
        if (startTP != TIME_POINT_INVALID) {
            filterFrom = std::min(startTP, filterFrom);
            filterTill = filterFrom;
        }

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

        // mlucki
        // Todo: calc here the duration to first upcoming alarm
        auto duration = seconds(0);
        auto actualTimePoint = TimePointNow();
        auto weekDay         = WeekdayIndexFromTimePoint(actualTimePoint);
        std::vector<AlarmsRecord> nearestAlarms;
        for (uint32_t i = 0; i < 7; i++) {
            for (const auto &record : records) {
                if (record.repeat == static_cast<uint32_t>(AlarmRepeat::never) ||
                    record.repeat == static_cast<uint32_t>(AlarmRepeat::everyday)) {
                    nearestAlarms.push_back(record);
                }
                else if (weekDay > 0 && weekDay < 6 && record.repeat == static_cast<uint32_t>(AlarmRepeat::weekDays)) {
                    nearestAlarms.push_back(record);
                }
                else {
                    OptionParser parser;
                    auto weekDayRepeatData = std::make_unique<WeekDaysRepeatData>();
                    auto weekDayData       = parser.setWeekDayOptions(record.repeat, std::move(weekDayRepeatData));
                    if (weekDayData->getData(weekDay)) {
                        nearestAlarms.push_back(record);
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

        // Apply snooze
        for (auto &alarm : nearestAlarms) {
            if (alarm.status > AlarmStatus::On) {
                alarm.time =
                    alarm.time + (static_cast<uint32_t>(alarm.status) - 1) * std::chrono::minutes(alarm.snooze);
            }
        }

        /*
        alarmsRecord = records->at(0);
        startTP       = alarmsRecord.date_from - minutes{alarmsRecord.reminder};
        auto duration = alarmsRecord.date_from - std::chrono::minutes{alarmsRecord.reminder} - TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }*/

        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool AlarmsTimeEvents::sendEventFiredQuery()
    {
        // mlucki
        // Todo: there is a place to set (in DB and optionally) the alarmsRecord as processed/invoked

        /*eventRecord.reminder_fired = TimePointNow();
        return DBServiceAPI::GetQuery(
            service(), db::Interface::Name::Alarms, std::make_unique<db::query::alarms::Edit>(alarmsRecord));*/

        return true;
    }

    void AlarmsTimeEvents::invokeEvent()
    {
        // mlucki
        /*std::unique_ptr<AlarmsRecordData> eventData = std::make_unique<AlarmsRecordData>();
        eventData->setDescription(style::window::calendar::name::event_reminder_window);
        auto event = std::make_shared<AlarmsRecord>(alarmsRecord);
        eventData->setData(event);*/

        /// Todo: restore invoke action after rework of application popups
        // app::manager::Controller::sendAction(service(), app::manager::actions::ShowReminder, std::move(eventData));
    }
} // namespace stm
