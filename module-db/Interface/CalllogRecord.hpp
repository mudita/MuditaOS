// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "ContactRecord.hpp"
#include "Databases/CalllogDB.hpp"
#include "Record.hpp"
#include "queries/calllog/QueryCalllogSetAllRead.hpp"
#include <PhoneNumber.hpp>
#include <utf8/UTF8.hpp>

#include <cstdint>
#include <vector>
#include <utility>

struct CalllogRecord : public Record
{
    PresentationType presentation        = PresentationType::PR_UNKNOWN;
    time_t date                          = 0;
    time_t duration                      = 0;
    CallType type                        = CallType::CT_NONE;
    UTF8 name                            = "";
    uint32_t contactId                   = 0;
    utils::PhoneNumber::View phoneNumber = utils::PhoneNumber::View();
    bool isRead                          = true;

    friend std::ostream &operator<<(std::ostream &out, const CalllogRecord &point);
    [[nodiscard]] std::string str() const;

    CalllogRecord() = default;
    CalllogRecord(const CallType type, const utils::PhoneNumber::View &number);
    CalllogRecord(const CalllogTableRow &tableRow);

    uint32_t getContactId() const;
};

enum class CalllogRecordField
{
    DATE,
    TYPE,
};

class CalllogRecordInterface : public RecordInterface<CalllogRecord, CalllogRecordField>
{
  public:
    CalllogRecordInterface(CalllogDB *CalllogDb, ContactsDB *contactsDb);

    bool Add(const CalllogRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(CalllogRecordField field, const char *str) override final;
    bool Update(const CalllogRecord &rec) override final;
    CalllogRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);
    bool SetAllRead();

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                      uint32_t limit,
                                                                      CalllogRecordField field,
                                                                      const char *str) override final;

    uint32_t GetLastID();

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    CalllogDB *calllogDB   = nullptr;
    ContactsDB *contactsDB = nullptr;
    ContactRecord GetContactRecordByID(uint32_t contactId);
    std::vector<CalllogRecord> GetByContactID(uint32_t id);

    std::unique_ptr<db::QueryResult> getQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> setAllReadQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getCountQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> removeQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getByContactIDQuery(std::shared_ptr<db::Query> query);
};
