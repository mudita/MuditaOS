// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventRecord.hpp"
#include <Databases/EventsDB.hpp>

#include <cstdint>

namespace db::query::alarmEvents
{
    class Add;
    class AddResult;
    class Edit;
    class EditResult;
    class Get;
    class GetBetweenDates;
    class GetBetweenDatesResult;
    class GetLimited;
    class GetLimitedResult;
    class GetNext;
    class GetNextResult;
    class GetRecurringBetweenDates;
    class GetRecurringBetweenDatesResult;
    class GetResult;
    class Remove;
    class RemoveResult;
} // namespace db::query::alarmEvents

struct AlarmEventRecord : public EventRecord
{
    std::string musicTone;
    bool enabled{false};
    uint32_t snoozeDuration{0};

    AlarmEventRecord() = default;
    AlarmEventRecord(uint32_t id,
                     const UTF8 &name,
                     TimePoint startDate,
                     uint32_t duration,
                     bool isAllDay,
                     const std::string &rruleText,
                     const std::string &musicTone,
                     bool enabled,
                     uint32_t snoozeDuration);

    explicit AlarmEventRecord(const AlarmEventsTableRow &aeRow);

    std::shared_ptr<EventRecord> getCopy() override;
    auto isValid() const -> bool;
};

class AlarmEventRecordInterface : public db::Interface
{
  public:
    explicit AlarmEventRecordInterface(EventsDB *eventsDB);
    ~AlarmEventRecordInterface();

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    EventsDB *eventsDB;

    std::unique_ptr<db::query::alarmEvents::AddResult> runQueryImplAdd(
        std::shared_ptr<db::query::alarmEvents::Add> query);
    std::unique_ptr<db::query::alarmEvents::EditResult> runQueryImplEdit(
        std::shared_ptr<db::query::alarmEvents::Edit> query);
    std::unique_ptr<db::query::alarmEvents::GetResult> runQueryImplGet(
        std::shared_ptr<db::query::alarmEvents::Get> query);
    std::unique_ptr<db::query::alarmEvents::GetBetweenDatesResult> runQueryImplGetBetweenDates(
        std::shared_ptr<db::query::alarmEvents::GetBetweenDates> query);
    std::unique_ptr<db::query::alarmEvents::GetLimitedResult> runQueryImplGetLimited(
        std::shared_ptr<db::query::alarmEvents::GetLimited> query);
    std::unique_ptr<db::query::alarmEvents::GetNextResult> runQueryImplGetNext(
        std::shared_ptr<db::query::alarmEvents::GetNext> query);
    std::unique_ptr<db::query::alarmEvents::GetRecurringBetweenDatesResult> runQueryImplGetRecurringBetweenDates(
        std::shared_ptr<db::query::alarmEvents::GetRecurringBetweenDates> query);
    std::unique_ptr<db::query::alarmEvents::RemoveResult> runQueryImplRemove(
        std::shared_ptr<db::query::alarmEvents::Remove> query);
};
