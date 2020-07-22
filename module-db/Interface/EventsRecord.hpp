#pragma once

#include "Common/Common.hpp"
#include "Databases/EventsDB.hpp"
#include "Record.hpp"

#include <utf8/UTF8.hpp>

#include <cstdint>
#include <vector>

// fw declarations
namespace db::query::events
{
    class Get;
    class GetResult;
    class Add;
    class AddResult;
    class Remove;
    class GetResult;
    class GetAll;
    class GetAllResult;
    //    class Increment;
    //    class IncrementResult;
    //    class Clear;
    //    class ClearResult;

} // namespace db::query::events

struct EventsRecord : public Record
{
    std::string title;
    std::string description;
    // date and time when event will begin
    uint32_t date_from = 0;
    // date and time when event will end
    uint32_t date_till = 0;
    // date and time of the event reminder
    uint32_t reminder = 0;
    // repeat the event daily
    uint32_t repeat = 0;

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
    virtual ~EventsRecordInterface() = default;

    bool Add(const EventsRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(EventsRecordField field, const char *str) override final;
    bool Update(const EventsRecord &rec) override final;
    EventsRecord GetByID(uint32_t id) override final;
    uint32_t GetCount() override final;
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<EventsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     EventsRecordField field,
                                                                     const char *str) override final;

    //    EventsRecord GetByKey(EventsRecord::Key key);

    std::unique_ptr<db::QueryResult> runQuery(const db::Query *query) override;

  private:
    EventsDB *eventsDb = nullptr;

    std::unique_ptr<db::query::events::GetResult> runQueryImpl(const db::query::events::Get *query);
    //    std::unique_ptr<db::query::events::IncrementResult> runQueryImpl(
    //        const db::query::events::Increment *query);
    //    std::unique_ptr<db::query::events::ClearResult> runQueryImpl(const db::query::events::Clear *query);
    std::unique_ptr<db::query::events::AddResult> runQueryImpl(const db::query::events::Add *query);
    //    std::unique_ptr<db::query::events::RemoveResult> runQueryImpl(const db::query::events::Remove *query);
    //    std::unique_ptr<db::query::events::UpdateResult> runQueryImpl(const db::query::events::Update *query);
    std::unique_ptr<db::query::events::GetAllResult> runQueryImpl(const db::query::events::GetAll *query);
};
