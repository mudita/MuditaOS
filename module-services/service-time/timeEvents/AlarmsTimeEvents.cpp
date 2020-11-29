// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/AlarmsTimeEvents.hpp>
#include <service-time/TimeEvents.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>

//#include <application-calendar/widgets/CalendarStyle.hpp>
/*#include <module-apps/application-calendar/ApplicationCalendar.hpp>
#include <module-apps/application-calendar/data/CalendarData.hpp>
#include <module-apps/application-calendar/data/dateCommon.hpp>*/

#include <module-db/queries/alarms/QueryAlarmsSelectFirstUpcoming.hpp>
#include <module-gui/gui/SwitchData.hpp>

//#include <queries/calendar/QueryEventsEdit.hpp>

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

        return DBServiceAPI::GetQuery(service(),
                                      db::Interface::Name::Alarms,
                                      std::make_unique<db::query::alarms::SelectFirstUpcoming>(filterFrom, filterTill));
    }

    uint32_t AlarmsTimeEvents::calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        auto firstUpcomingQuery =
            dynamic_cast<db::query::alarms::SelectFirstUpcomingResult *>(nextEventQueryResult.get());
        if (firstUpcomingQuery == nullptr) {
            return 0;
        }

        std::unique_ptr<std::vector<AlarmsRecord>> records = firstUpcomingQuery->getResult();
        if (records->size() == 0) {
            return 0;
        }

        alarmsRecord = records->at(0);

        // mlucki
        auto duration = minutes(10);
        // mlucki
        /*startTP       = alarmsRecord.date_from - minutes{alarmsRecord.reminder};
        auto duration = alarmsRecord.date_from - std::chrono::minutes{alarmsRecord.reminder} - TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }*/

        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool AlarmsTimeEvents::sendEventFiredQuery()
    {
        // mlucki
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

        /// Todo: restore action after rework of application popups
        // app::manager::Controller::sendAction(service(), app::manager::actions::ShowReminder, std::move(eventData));
    }
} // namespace stm
