// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreadRecord.hpp"
#include "SMSRecord.hpp"
#include "ContactRecord.hpp"

#include <queries/messages/threads/QueryThreadGetByID.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <queries/messages/threads/QueryThreadRemove.hpp>
#include <queries/messages/threads/QueryThreadsGet.hpp>
#include <queries/messages/threads/QueryThreadsGetForList.hpp>
#include <queries/messages/threads/QueryThreadsSearchForList.hpp>
#include <queries/messages/threads/QueryThreadsGetCount.hpp>

#include <cassert>
#include <log/log.hpp>

ThreadRecordInterface::ThreadRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb)
    : smsDB(smsDb), contactsDB(contactsDb)
{}

bool ThreadRecordInterface::Add(const ThreadRecord &rec)
{
    auto result = smsDB->threads.add(ThreadsTableRow{Record(rec.ID),
                                                     .date           = rec.date,
                                                     .msgCount       = rec.msgCount,
                                                     .unreadMsgCount = rec.unreadMsgCount,
                                                     .numberID       = rec.numberID,
                                                     .snippet        = rec.snippet,
                                                     .type           = rec.type});

    return result;
}

bool ThreadRecordInterface::RemoveByID(uint32_t id)
{
    auto result = smsDB->threads.removeById(id);
    if (!result) {
        return false;
    }

    SMSRecordInterface smsRecordInterface(smsDB, contactsDB);
    return smsRecordInterface.RemoveByField(SMSRecordField::ThreadID, std::to_string(id).c_str());
}

bool ThreadRecordInterface::Update(const ThreadRecord &rec)
{
    return smsDB->threads.update(ThreadsTableRow{Record(rec.ID),
                                                 .date           = rec.date,
                                                 .msgCount       = rec.msgCount,
                                                 .unreadMsgCount = rec.unreadMsgCount,
                                                 .numberID       = rec.numberID,
                                                 .snippet        = rec.snippet,
                                                 .type           = rec.type

    });
}

uint32_t ThreadRecordInterface::GetCount()
{
    return smsDB->threads.count();
}

uint32_t ThreadRecordInterface::GetCount(EntryState state)
{
    return smsDB->threads.count(state);
}

std::unique_ptr<std::vector<ThreadRecord>> ThreadRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto records = std::make_unique<std::vector<ThreadRecord>>();

    auto result = smsDB->threads.getLimitOffset(offset, limit);

    for (const auto &w : result) {
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

    ThreadsTableFields threadsField = ThreadsTableFields();
    switch (field) {
    case ThreadRecordField::NumberID: {
        threadsField = ThreadsTableFields::NumberID;
    } break;
    default:
        LOG_ERROR("Invalid field type %u", static_cast<unsigned>(field));
        return records;
    }

    auto result = smsDB->threads.getLimitOffsetByField(offset, limit, threadsField, str);
    for (const auto &w : result) {
        records->push_back(w);
    }

    return records;
}

ThreadRecord ThreadRecordInterface::GetByID(uint32_t id)
{
    auto rec = smsDB->threads.getById(id);
    if (!rec.isValid()) {
        return ThreadRecord();
    }

    return ThreadRecord(rec);
}

ThreadRecord ThreadRecordInterface::GetByNumber(const utils::PhoneNumber::View &phoneNumber)
{
    auto contactInterface = ContactRecordInterface(contactsDB);
    auto match            = contactInterface.MatchByNumber(phoneNumber);

    if (!match.has_value()) {
        return ThreadRecord();
    }

    auto threadRec = GetLimitOffsetByField(0, 1, ThreadRecordField::NumberID, std::to_string(match->numberId).c_str());

    if (threadRec->size() == 0) {
        return ThreadRecord();
    }

    return threadRec->at(0);
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::ThreadsSearchForList)) {
        return threadSearchForListQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::MarkAsRead)) {
        return markAsReadQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadsGet)) {
        return threadsGetQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadsGetWithTotalCount)) {
        return threadsGetQueryWithTotalCount(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadsGetForList)) {
        return threadsGetForListQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadGetByID)) {
        return threadGetByIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadGetByNumber)) {
        return threadGetByNumberQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadRemove)) {
        return threadRemoveQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ThreadGetCount)) {
        return threadsGetCount(query);
    }

    return nullptr;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadSearchForListQuery(
    const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadsSearchForList *>(query.get());
    auto dbResult = smsDB->threads.getBySMSQuery(localQuery->textToSearch, localQuery->offset, localQuery->limit);

    auto records = std::vector<ThreadRecord>(dbResult.second.begin(), dbResult.second.end());
    auto count   = dbResult.first;

    std::vector<ContactRecord> contacts;

    auto contactDb = ContactRecordInterface(contactsDB);
    for (const auto &record : records) {
        if (auto contact = contactDb.GetByNumberID(record.numberID); contact.has_value()) {
            contacts.push_back(contact.value());
        }
    }

    auto response = std::make_unique<db::query::ThreadsSearchResultForList>(records, contacts, count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::markAsReadQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::MarkAsRead *>(query.get());
    auto record           = GetByID(localQuery->id);
    auto result           = false;

    if (record.isValid()) {
        LOG_DEBUG("Query read: %d", static_cast<int>(localQuery->read));
        record.unreadMsgCount = localQuery->read == db::query::MarkAsRead::Read::True ? 0 : 1;
        result                = Update(record);
    }

    auto response = std::make_unique<db::query::MarkAsReadResult>(result);
    response->setRequestQuery(query);
    response->setRecordID(localQuery->id);
    return response;
}

std::vector<ThreadRecord> ThreadRecordInterface::getThreads(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadsGet *>(query.get());
    auto dbResult         = smsDB->threads.getLimitOffset(localQuery->offset, localQuery->limit);
    return std::vector<ThreadRecord>(dbResult.begin(), dbResult.end());
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadsGetQuery(const std::shared_ptr<db::Query> &query)
{
    auto response = std::make_unique<db::query::ThreadsGetResults>(std::move(getThreads(query)));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadsGetQueryWithTotalCount(
    const std::shared_ptr<db::Query> &query)
{
    auto count    = smsDB->threads.count();
    auto response = std::make_unique<db::query::ThreadsGetResultsWithTotalCount>(std::move(getThreads(query)), count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadsGetForListQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadsGetForList *>(query.get());

    auto dbResult = smsDB->threads.getLimitOffset(localQuery->offset, localQuery->limit);
    auto records  = std::vector<ThreadRecord>(dbResult.begin(), dbResult.end());

    std::vector<ContactRecord> contacts;
    std::vector<utils::PhoneNumber::View> numbers;

    auto contactDb = ContactRecordInterface(contactsDB);
    for (const auto &record : records) {
        if (auto contact = contactDb.GetByNumberID(record.numberID); contact.has_value()) {
            contacts.push_back(contact.value());
        }
        numbers.push_back(contactDb.GetNumberById(record.numberID));
    }

    auto count = smsDB->threads.count();

    auto response = std::make_unique<db::query::ThreadsGetForListResults>(records, contacts, numbers, count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadGetByIDQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadGetByID *>(query.get());

    const auto ret = GetByID(localQuery->id);
    auto response  = std::make_unique<db::query::ThreadGetByIDResult>(ret.isValid() ? std::optional<ThreadRecord>{ret}
                                                                                    : std::nullopt);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadGetByNumberQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadGetByNumber *>(query.get());

    auto response = std::make_unique<db::query::ThreadGetByNumberResult>(GetByNumber(localQuery->getNumber()));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadRemoveQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadRemove *>(query.get());

    const auto result = RemoveByID(localQuery->id);
    auto response     = std::make_unique<db::query::ThreadRemoveResult>(result);
    response->setRequestQuery(query);
    response->setRecordID(localQuery->id);
    return response;
}

std::unique_ptr<db::QueryResult> ThreadRecordInterface::threadsGetCount(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::ThreadGetCount *>(query.get());

    auto count    = GetCount(localQuery->getState());
    auto response = std::make_unique<db::query::ThreadGetCountResult>(localQuery->getState(), count);
    response->setRequestQuery(query);
    return response;
}
