/*
 * @file CalllogRecord.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Record
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "../Common/Common.hpp"
#include "../Databases/CalllogDB.hpp"
#include "ContactRecord.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include <stdint.h>

struct CalllogRecord : public Record
{
    UTF8 number = "";
    PresentationType presentation = PresentationType::PR_UNKNOWN;
    time_t date = 0;
    time_t duration = 0;
    CallType type = CallType::CT_NONE;
    UTF8 name = "";
    UTF8 contactId = "";

    friend std::ostream &operator<<(std::ostream &out, const CalllogRecord &point);

    CalllogRecord() = default;
    ~CalllogRecord() = default;
    CalllogRecord(const CalllogTableRow &tableRow, const UTF8 &num, const UTF8 &name);
};

enum class CalllogRecordField{
    DATE,
	TYPE,
 };

/*
 *
 */
class CalllogRecordInterface : public RecordInterface<CalllogRecord,CalllogRecordField > {
public:
  CalllogRecordInterface(CalllogDB *CalllogDb, ContactsDB *contactsDb);
  virtual ~CalllogRecordInterface();

  bool Add(const CalllogRecord &rec) override final;
  bool RemoveByID(uint32_t id) override final;
  bool RemoveByField(CalllogRecordField field, const char *str) override final;
  bool Update(const CalllogRecord &rec) override final;
  CalllogRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffsetByField(uint32_t offset, uint32_t limit, CalllogRecordField field,
                                                                      const char *str) override final;

    uint32_t GetLastID();

  private:
    CalllogDB *calllogDB = nullptr;
    ContactsDB *contactsDB = nullptr;
    ContactRecord GetContactRecordByID(const UTF8 &contactId);
};
