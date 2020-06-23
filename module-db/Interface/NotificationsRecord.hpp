#pragma once

#include "Common/Common.hpp"
#include "Databases/NotificationsDB.hpp"
#include "Record.hpp"

#include <utf8/UTF8.hpp>

#include <cstdint>
#include <vector>

// fw declarations
namespace db::query::notifications
{
    class QueryGet;
    class QueryGetResult;
    class QueryIncrement;
    class QueryIncrementResult;
    class QueryClear;
    class QueryClearResult;
    class QueryGetAll;
    class QueryGetAllResult;
} // namespace db::query::notifications

struct NotificationsRecord : public Record
{
    enum class Key
    {
        NotValidKey = 0,
        Calls,
        Sms,
        NumberOfKeys // do not use directly
    };

    Key key        = Key::NotValidKey;
    uint32_t value = 0;

    friend std::ostream &operator<<(std::ostream &out, const NotificationsRecord &point);

    NotificationsRecord()  = default;
    ~NotificationsRecord() = default;
    NotificationsRecord(const NotificationsTableRow &tableRow);

    bool isValidRecord() const;
    bool gotValidKey() const;

    static bool isValidKey(Key key);
};

enum class NotificationsRecordField
{
    key
};

class NotificationsRecordInterface : public RecordInterface<NotificationsRecord, NotificationsRecordField>
{
  public:
    NotificationsRecordInterface(NotificationsDB *notificationsDb);
    virtual ~NotificationsRecordInterface() = default;

    bool Add(const NotificationsRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(NotificationsRecordField field, const char *str) override final;
    bool Update(const NotificationsRecord &rec) override final;
    NotificationsRecord GetByID(uint32_t id) override final;
    uint32_t GetCount() override final;
    std::unique_ptr<std::vector<NotificationsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<NotificationsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                            uint32_t limit,
                                                                            NotificationsRecordField field,
                                                                            const char *str) override final;

    NotificationsRecord GetByKey(NotificationsRecord::Key key);

    std::unique_ptr<db::QueryResult> runQuery(const db::Query *query) override;

  private:
    NotificationsDB *notificationsDb = nullptr;

    std::unique_ptr<db::query::notifications::QueryGetResult> runQueryImpl(
        const db::query::notifications::QueryGet *query);
    std::unique_ptr<db::query::notifications::QueryIncrementResult> runQueryImpl(
        const db::query::notifications::QueryIncrement *query);
    std::unique_ptr<db::query::notifications::QueryClearResult> runQueryImpl(
        const db::query::notifications::QueryClear *query);
    std::unique_ptr<db::query::notifications::QueryGetAllResult> runQueryImpl(
        const db::query::notifications::QueryGetAll *query);
};
