// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventsDBRepository.hpp"
#include <service-db/DBServiceAPI.hpp>

#include <module-db/queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsGetEnabled.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsGetInRange.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsRemove.hpp>
#include <module-db/queries/alarm_events/QueryAlarmEventsToggleAll.hpp>

namespace alarms
{
    AlarmEventsDBRepository::AlarmEventsDBRepository(sys::Service *service) : service{service}
    {}

    auto AlarmEventsDBRepository::getAlarmEvent(const std::uint32_t alarmId, const OnGetAlarmEventCallback &callback)
        -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::Get, db::query::alarmEvents::GetResult, OnGetAlarmEventCallback>(
            callback, alarmId);
    }

    auto AlarmEventsDBRepository::getAlarmEnabledEvents(const OnGetAlarmEventsCallback &callback) -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::GetEnabled,
                              db::query::alarmEvents::GetEnabledResult,
                              OnGetAlarmEventsCallback>(callback);
    }

    auto AlarmEventsDBRepository::getAlarmEventsInRange(std::uint32_t offset,
                                                        std::uint32_t limit,
                                                        const OnGetAlarmEventsInRangeCallback &callback) -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::GetInRange,
                              db::query::alarmEvents::GetInRangeResult,
                              OnGetAlarmEventsInRangeCallback>(callback, offset, limit);
    }

    auto AlarmEventsDBRepository::addAlarmEvent(const AlarmEventRecord &alarmEvent,
                                                const OnAddAlarmEventCallback &callback) -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::Add, db::query::alarmEvents::AddResult, OnAddAlarmEventCallback>(
            callback, alarmEvent);
    }

    auto AlarmEventsDBRepository::updateAlarmEvent(const AlarmEventRecord &alarmEvent,
                                                   const OnUpdateAlarmEventCallback &callback) -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::Edit,
                              db::query::alarmEvents::EditResult,
                              OnUpdateAlarmEventCallback>(callback, alarmEvent);
    }

    auto AlarmEventsDBRepository::removeAlarmEvent(const std::uint32_t id, const OnRemoveAlarmEventCallback &callback)
        -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::Remove,
                              db::query::alarmEvents::RemoveResult,
                              OnRemoveAlarmEventCallback>(callback, id);
    }

    auto AlarmEventsDBRepository::toggleAll(bool toggle, const OnToggleAll &callback) -> void
    {
        sendQueryWithCallback<db::query::alarmEvents::ToggleAll, db::query::alarmEvents::ToggleAllResult, OnToggleAll>(
            callback, toggle);
    }

    template <typename requestT, typename responseT, typename callbackT, typename... Types>
    void AlarmEventsDBRepository::sendQueryWithCallback(const callbackT &callback, Types... queryArgs)
    {
        auto query = std::make_unique<requestT>(queryArgs...);

        query->setQueryListener(db::QueryCallback::fromFunction([callback](auto response) {
            auto result = dynamic_cast<responseT *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->getResult());
            }
            return true;
        }));
        DBServiceAPI::GetQuery(service, db::Interface::Name::AlarmEvents, std::move(query));
    }

} // namespace alarms
