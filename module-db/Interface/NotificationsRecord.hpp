#pragma once

#include "Common/Common.hpp"
#include "Databases/NotificationsDB.hpp"
#include "Record.hpp"

#include <utf8/UTF8.hpp>

#include <cstdint>
#include <vector>

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

  private:
    NotificationsDB *notificationsDb = nullptr;
};
