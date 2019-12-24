
/*
 * @file ThreadRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_THREADRECORD_HPP
#define PUREPHONE_THREADRECORD_HPP

#include "Record.hpp"
#include <stdint.h>
#include "utf8/UTF8.hpp"
#include "../Databases/SmsDB.hpp"
#include "../Databases/ContactsDB.hpp"
#include "../Common/Common.hpp"


struct ThreadRecord{
    uint32_t dbID = 0;
    uint32_t date = 0;
    uint32_t msgCount = 0;
    uint32_t msgRead = 0;
    UTF8 snippet = "";
    SMSType type = SMSType::ALL;
    uint32_t contactID = 0;

    ThreadRecord() = default;
    ThreadRecord(const ThreadsTableRow &rec)
    {
        dbID = rec.ID;
        date = rec.date;
        msgCount = rec.msgCount;
        msgRead = rec.msgRead;
        snippet = rec.snippet;
        type = rec.type;
        contactID = rec.contactID;
    }
};

enum class ThreadRecordField{
    ContactID,
};

class ThreadRecordInterface : public RecordInterface<ThreadRecord,ThreadRecordField>{
public:

    ThreadRecordInterface(SmsDB* smsDb,ContactsDB* contactsDb);
    ~ThreadRecordInterface();

    bool Add(const ThreadRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(const ThreadRecord& rec) override final;
    ThreadRecord GetByID(uint32_t id) override final;
    ThreadRecord GetByContact(uint32_t contact_id);

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,ThreadRecordField field, const char* str) override final;
private:
   SmsDB* smsDB;
   ContactsDB* contactsDB;

};


#endif //PUREPHONE_THREADRECORD_HPP
