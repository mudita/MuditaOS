// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventRecord.hpp"
#include <databases/EventsDB.hpp>

#include <cstdint>

namespace db::query::alarmEvents
{
    class Add;
    class AddResult;
    class Edit;
    class EditResult;
    class Get;
    class GetEnabled;
    class GetEnabledResult;
    class GetInRange;
    class GetInRangeResult;
    class GetNext;
    class GetNextResult;
    class GetRecurring;
    class GetRecurringResult;
    class GetResult;
    class Remove;
    class RemoveResult;
    class ToggleAll;
    class ToggleAllResult;
} // namespace db::query::alarmEvents

using namespace std::chrono_literals;
struct AlarmTime
{
    std::chrono::hours hourOfDay;
    std::chrono::minutes minuteOfHour;
    AlarmTime(std::chrono::hours hourOfDay, std::chrono::minutes minuteOfHour)
        : hourOfDay{hourOfDay}, minuteOfHour{minuteOfHour}
    {}
};
struct AlarmEventRecord : public EventRecord
{
    AlarmTime alarmTime{0h, 0min};
    std::string musicTone;
    bool enabled{false};
    uint32_t snoozeDuration{0};
    std::string rruleText;

    AlarmEventRecord() = default;
    AlarmEventRecord(uint32_t id,
                     const AlarmTime alarmTime,
                     const std::string &musicTone,
                     bool enabled,
                     uint32_t snoozeDuration,
                     const std::string &rruleText);

    explicit AlarmEventRecord(const AlarmEventsTableRow &aeRow);

    std::shared_ptr<EventRecord> getCopy() override;
    auto isValid() const -> bool;
    auto getNextSingleEvent(TimePoint from) -> SingleEventRecord;
};

class AlarmEventRecordInterface : public db::Interface
{
  public:
    explicit AlarmEventRecordInterface(EventsDB *eventsDB);
    virtual ~AlarmEventRecordInterface() noexcept = default;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    EventsDB *eventsDB;

    std::unique_ptr<db::query::alarmEvents::AddResult> runQueryImplAdd(
        std::shared_ptr<db::query::alarmEvents::Add> query);
    std::unique_ptr<db::query::alarmEvents::EditResult> runQueryImplEdit(
        std::shared_ptr<db::query::alarmEvents::Edit> query);
    std::unique_ptr<db::query::alarmEvents::GetResult> runQueryImplGet(
        std::shared_ptr<db::query::alarmEvents::Get> query);
    std::unique_ptr<db::query::alarmEvents::GetEnabledResult> runQueryImplGetEnabled(
        std::shared_ptr<db::query::alarmEvents::GetEnabled> query);
    std::unique_ptr<db::query::alarmEvents::GetInRangeResult> runQueryImplGetInRange(
        std::shared_ptr<db::query::alarmEvents::GetInRange> query);
    std::unique_ptr<db::query::alarmEvents::RemoveResult> runQueryImplRemove(
        std::shared_ptr<db::query::alarmEvents::Remove> query);
    std::unique_ptr<db::query::alarmEvents::ToggleAllResult> runQueryImplRemove(
        std::shared_ptr<db::query::alarmEvents::ToggleAll> query);
};
