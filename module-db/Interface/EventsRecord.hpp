#pragma once

#include "Common/Common.hpp"
#include "Databases/EventsDB.hpp"
#include "Record.hpp"
#include <utf8/UTF8.hpp>
#include <cstdint>
#include <vector>
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
    class Add;
    class AddResult;
    class Remove;
    class RemoveResult;
    class Edit;
    class EditResult;

} // namespace db::query::events

enum class RepeatOption
{
    Never    = 0,
    Daily    = 1,
    Weekly   = 2,
    TwoWeeks = 3,
    Month    = 4,
    Year     = 5
};

struct EventsRecord : public Record
{
    std::string title;
    TimePoint date_from;
    TimePoint date_till;
    uint32_t reminder = 0;
    uint32_t repeat    = 0;

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
    bool RemoveByField(EventsRecordField field, const char *str) override final;
    bool Update(const EventsRecord &rec);
    EventsRecord GetByID(uint32_t id) override final;
    uint32_t GetCount() override final;
    std::unique_ptr<std::vector<EventsRecord>> Select(TimePoint filter_from, TimePoint filter_till);
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     EventsRecordField field,
                                                                     const char *str) override final;
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffsetByDate(uint32_t offset, uint32_t limit);

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    EventsDB *eventsDb = nullptr;

    std::unique_ptr<db::query::events::GetResult> runQueryImpl(const db::query::events::Get *query);
    std::unique_ptr<db::query::events::GetAllResult> runQueryImpl(const db::query::events::GetAll *query);
    std::unique_ptr<db::query::events::GetAllLimitedResult> runQueryImpl(const db::query::events::GetAllLimited *query);
    std::unique_ptr<db::query::events::GetFilteredResult> runQueryImpl(const db::query::events::GetFiltered *query);
    std::unique_ptr<db::query::events::AddResult> runQueryImpl(const db::query::events::Add *query);
    std::unique_ptr<db::query::events::RemoveResult> runQueryImpl(const db::query::events::Remove *query);
    std::unique_ptr<db::query::events::EditResult> runQueryImpl(const db::query::events::Edit *query);
};
