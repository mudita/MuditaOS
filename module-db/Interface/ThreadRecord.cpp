
/*
 * @file ThreadRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ThreadRecord.hpp"
#include "../Databases/SmsDB.hpp"

bool ThreadRecordInterface::Add(const ThreadRecord &rec) {
    auto smsDB = std::make_unique<SmsDB>();

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
    auto smsDB = std::make_unique<SmsDB>();

    auto ret = smsDB->threads.RemoveByID(id);
    if (ret == false) {
        return false;
    }

    //TODO: remove SMS records from SMS tables
}

bool ThreadRecordInterface::Update(const ThreadRecord &rec) {
    auto smsDB = std::make_unique<SmsDB>();

    auto ret = smsDB->threads.Update(ThreadsTableRow{
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
    auto smsDB = std::make_unique<SmsDB>();

    return smsDB->threads.GetCount();
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto smsDB = std::make_unique<SmsDB>();

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
                                                                                        ContactRecordField field,
                                                                                        const char *str) {
    auto smsDB = std::make_unique<SmsDB>();
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    switch (field) {
        case ContactRecordField::Date: {
            auto ret = smsDB->threads.GetLimitOffsetByField(offset, limit,ThreadsTableFields::Date,str);

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
}