
/*
 * @file ThreadRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ThreadRecord.hpp"
#include "SMSRecord.hpp"


ThreadRecordInterface::ThreadRecordInterface(SmsDB* smsDb,ContactsDB* contactsDb): smsDB(smsDb),contactsDB(contactsDb) {
}

ThreadRecordInterface::~ThreadRecordInterface() {

}

bool ThreadRecordInterface::Add(const ThreadRecord &rec) {
    auto ret = smsDB->threads.Add(ThreadsTableRow{
            .date = rec.date,
            .msgCount=rec.msgCount,
            .msgRead=rec.msgRead,
            .contactID=rec.contactID,
            .snippet=rec.snippet,
            .type=rec.type

    });

    return ret;
}

bool ThreadRecordInterface::RemoveByID(uint32_t id) {

    auto ret = smsDB->threads.RemoveByID(id);
    if (ret == false) {
        return false;
    }

    SMSRecordInterface smsRecordInterface(smsDB,contactsDB);
    return smsRecordInterface.RemoveByField(SMSRecordField::ThreadID,std::to_string(id).c_str());
}

bool ThreadRecordInterface::Update(const ThreadRecord &rec) {
    return smsDB->threads.Update(ThreadsTableRow{
            .ID = rec.dbID,
            .date = rec.date,
            .msgCount=rec.msgCount,
            .msgRead=rec.msgRead,
            .contactID=rec.contactID,
            .snippet=rec.snippet,
            .type=rec.type

    });

}

uint32_t ThreadRecordInterface::GetCount() {

    return smsDB->threads.GetCount();
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    auto ret = smsDB->threads.GetLimitOffset(offset, limit);

    for (const auto &w: ret) {
        records->push_back(ThreadRecord{
                .dbID = w.ID,
                .date = w.date,
                .msgCount=w.msgCount,
                .msgRead=w.msgRead,
                .snippet=w.snippet,
                .type=w.type,
                .contactID=w.contactID
        });
    }

    return records;
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffsetByField(uint32_t offset, uint32_t limit,
                                                                                        ThreadRecordField field,
                                                                                        const char *str) {
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    switch (field) {
        case ThreadRecordField::ContactID: {
            auto ret = smsDB->threads.GetLimitOffsetByField(offset, limit,ThreadsTableFields::ContactID,str);

            for(const auto &w: ret){
                records->push_back(ThreadRecord{
                        .dbID = w.ID,
                        .date = w.date,
                        .msgCount=w.msgCount,
                        .msgRead=w.msgRead,
                        .snippet=w.snippet,
                        .type=w.type,
                        .contactID=w.contactID
                });
            }
        }
            break;

    }

    return records;
}

ThreadRecord ThreadRecordInterface::GetByID(uint32_t id) {

    auto rec = smsDB->threads.GetByID(id);

    return ThreadRecord{
            .dbID = rec.ID,
            .date = rec.date,
            .msgCount=rec.msgCount,
            .msgRead=rec.msgRead,
            .snippet=rec.snippet,
            .type=rec.type,
            .contactID=rec.contactID
    };
}