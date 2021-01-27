// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Common/Common.hpp"
#include "module-db/Databases/EventsDB.hpp"
#include "Record.hpp"
#include <utf8/UTF8.hpp>
#include <cstdint>
#include <vector>
#include <variant>
#include <module-apps/application-calendar/data/dateCommon.hpp>

// fw declarations
namespace db::query::events
{
    class Get;
    class GetResult;
    class GetAll;
    class GetAllResult;
    class GetAllLimited;
    class GetAllLimitedResult;
    class GetFiltered;
    class GetFilteredResult;
    class GetFilteredByDay;
    class GetFilteredByDayResult;
    class Add;
    class AddResult;
    class Remove;
    class RemoveResult;
    class RemoveICS;
    class RemoveICSResult;
    class Edit;
    class EditResult;
    class EditICS;
    class EditICSResult;
    class SelectFirstUpcoming;
    class SelectFirstUpcomingResult;
} // namespace db::query::events

struct EventsRecord : public Record
{
    std::string UID;
    std::string title;
    TimePoint date_from;
    TimePoint date_till;
    uint32_t reminder = 0;
    uint32_t repeat   = 0;
    TimePoint reminder_fired;
    std::string provider_type;
    std::string provider_id;
    std::string provider_iCalUid;

    EventsRecord()  = default;
    ~EventsRecord() = default;
    EventsRecord(const EventsTableRow &tableRow);
};

enum class EventsRecordField
{
    start,
    end,
    reminder
};

class EventsRecordInterface : public RecordInterface<EventsRecord, EventsRecordField>
{
  public:
    EventsRecordInterface(EventsDB *eventsDb);
    ~EventsRecordInterface() override = default;

    bool Add(const EventsRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByUID(const std::string &UID);
    bool RemoveByField(EventsRecordField field, const char *str) override final;
    bool Update(const EventsRecord &rec) override final;
    bool UpdateByUID(const EventsRecord &rec);
    EventsRecord GetByID(uint32_t id) override final;
    EventsRecord GetByUID(const std::string &uid);
    uint32_t GetCount() override final;
    uint32_t GetCountFiltered(TimePoint from, TimePoint till);
    uint32_t GetCountFilteredByDay(TimePoint filter);
    std::vector<EventsRecord> Select(TimePoint filter_from, TimePoint filter_till, uint32_t offset, uint32_t limit);
    std::vector<EventsRecord> Select(TimePoint date, uint32_t offset, uint32_t limit);
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     EventsRecordField field,
                                                                     const char *str) override final;
    std::vector<EventsRecord> GetLimitOffsetByDate(uint32_t offset, uint32_t limit);
    std::vector<EventsRecord> SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till);

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    EventsDB *eventsDb = nullptr;

    std::unique_ptr<db::query::events::GetResult> runQueryImplGetResult(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::GetAllResult> runQueryImplGetAllResult(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::GetAllLimitedResult> runQueryImplGetAllLimitedResult(
        std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::GetFilteredResult> runQueryImplGetFilteredResult(
        std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::GetFilteredByDayResult> runQueryImplGetFilteredByDayResult(
        std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::AddResult> runQueryImplAdd(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::RemoveResult> runQueryImplRemove(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::RemoveICSResult> runQueryImplRemoveICS(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::EditResult> runQueryImplEdit(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::EditICSResult> runQueryImplEditICS(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::events::SelectFirstUpcomingResult> runQueryImplSelectFirstUpcoming(
        std::shared_ptr<db::Query> query);
};
