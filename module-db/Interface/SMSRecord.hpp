
/*
 * @file SMSRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SMSRECORD_HPP
#define PUREPHONE_SMSRECORD_HPP

#include "Record.hpp"
#include <stdint.h>
#include "../Databases/SmsDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct SMSRecord{
    uint32_t dbID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8 number;
    UTF8 body;
    bool isRead;
    SMSType type;
    uint32_t threadID;
    uint32_t contactID;
};

enum class SMSRecordField{
    Number,
    ThreadID,
    ContactID
};

class SMSRecordInterface : public RecordInterface<SMSRecord,SMSRecordField > {
public:

    bool Add(const SMSRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSRecordField field,const char* str) override final;
    bool Update(const SMSRecord& rec) override final;
    SMSRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,SMSRecordField field, const char* str) override final;

private:
    const uint32_t snippetLength = 45;
    std::unique_ptr<SmsDB> smsDB;

};


#endif //PUREPHONE_SMSRECORD_HPP
