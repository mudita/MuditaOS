
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
#include "queries/sms/QuerySMSSearch.hpp"
#include <cassert>
#include <log/log.hpp>

ThreadRecordInterface::ThreadRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb)
    : smsDB(smsDb), contactsDB(contactsDb)
{}

ThreadRecordInterface::~ThreadRecordInterface()
{}

bool ThreadRecordInterface::Add(const ThreadRecord &rec)
{
    auto ret = smsDB->threads.Add(ThreadsTableRow{.date      = rec.date,
                                                  .msgCount  = rec.msgCount,
                                                  .msgRead   = rec.msgRead,
                                                  .contactID = rec.contactID,
                                                  .snippet   = rec.snippet,
                                                  .type      = rec.type

    });

    return ret;
}

bool ThreadRecordInterface::RemoveByID(uint32_t id)
{

    auto ret = smsDB->threads.RemoveByID(id);
    if (ret == false) {
        return false;
    }

    SMSRecordInterface smsRecordInterface(smsDB, contactsDB);
    return smsRecordInterface.RemoveByField(SMSRecordField::ThreadID, std::to_string(id).c_str());
}

bool ThreadRecordInterface::Update(const ThreadRecord &rec)
{
    return smsDB->threads.Update(ThreadsTableRow{.ID        = rec.dbID,
                                                 .date      = rec.date,
                                                 .msgCount  = rec.msgCount,
                                                 .msgRead   = rec.msgRead,
                                                 .contactID = rec.contactID,
                                                 .snippet   = rec.snippet,
                                                 .type      = rec.type

    });
}

uint32_t ThreadRecordInterface::GetCount()
{

    return smsDB->threads.GetCount();
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    auto ret = smsDB->threads.GetLimitOffset(offset, limit);

    for (const auto &w : ret) {
        records->push_back(w);
    }

    return records;
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                        uint32_t limit,
                                                                                        ThreadRecordField field,
                                                                                        const char *str)
{
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    switch (field) {
    case ThreadRecordField::ContactID: {
        auto ret = smsDB->threads.GetLimitOffsetByField(offset, limit, ThreadsTableFields::ContactID, str);

        for (const auto &w : ret) {
            records->push_back(w);
        }
    } break;
    }

    return records;
}

ThreadRecord ThreadRecordInterface::GetByID(uint32_t id)
{

    auto rec = smsDB->threads.GetByID(id);
    if (rec.ID == 0) {
        return ThreadRecord();
    }

    return ThreadRecord(rec);
}

ThreadRecord ThreadRecordInterface::GetByContact(uint32_t contact_id)
{
    auto ret =
        smsDB->threads.GetLimitOffsetByField(0, 1, ThreadsTableFields::ContactID, std::to_string(contact_id).c_str());
    if (ret.size() == 0) {
        ThreadRecord re;
        re.contactID = contact_id;
        if (!Add(re)) {
            LOG_ERROR("There is no thread but we cant add it");
            assert(0);
        }

        ret = smsDB->threads.GetLimitOffsetByField(
            0, 1, ThreadsTableFields::ContactID, std::to_string(contact_id).c_str());
    }
    ThreadRecord a = ret[0];
    return a;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::runQuery(const db::Query *query)
{
    if (const auto local_query = dynamic_cast<const db::query::SMSSearch *>(query)) {
        return runQueryImpl(local_query);
    }
    return nullptr;
}

std::unique_ptr<db::query::SMSSearchResult> ThreadRecordInterface::runQueryImpl(const db::query::SMSSearch *query)
{
    auto db_result = smsDB->threads.getBySMSQuery(query->text, query->starting_postion, query->depth);
    return std::make_unique<db::query::SMSSearchResult>(db_result.first, db_result.second);
}
