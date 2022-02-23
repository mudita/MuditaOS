// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSRecord.hpp"
#include "Common/Query.hpp"
#include "ContactRecord.hpp"
#include "ThreadRecord.hpp"
#include "queries/messages/sms/QuerySMSAdd.hpp"
#include "queries/messages/sms/QuerySMSGet.hpp"
#include "queries/messages/sms/QuerySMSGetByID.hpp"
#include "queries/messages/sms/QuerySMSGetByText.hpp"
#include "queries/messages/sms/QuerySMSGetCount.hpp"
#include "queries/messages/sms/QuerySMSRemove.hpp"
#include "queries/messages/sms/QuerySMSUpdate.hpp"
#include "queries/messages/sms/QuerySMSSearchByType.hpp"
#include "queries/messages/sms/QuerySMSGetByThreadID.hpp"
#include "queries/messages/sms/QuerySMSGetCountByThreadID.hpp"
#include "queries/messages/sms/QuerySMSGetLastByThreadID.hpp"
#include <queries/messages/sms/QuerySMSGetForList.hpp>

#include <log/log.hpp>

#include <PhoneNumber.hpp>
#include <optional>

SMSRecordInterface::SMSRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb) : smsDB(smsDb), contactsDB(contactsDb)
{}

bool SMSRecordInterface::Add(const SMSRecord &rec)
{
    ContactRecordInterface contactInterface(contactsDB);
    auto contactMatch = contactInterface.MatchByNumber(rec.number, ContactRecordInterface::CreateTempContact::True);
    if (!contactMatch.has_value()) {
        LOG_ERROR("Cannot find contact, for number %s", rec.number.getEntered().c_str());
        return false;
    }
    auto numberID  = contactMatch->numberId;

    ThreadRecordInterface threadInterface(smsDB, contactsDB);
    auto threadRec =
        threadInterface.GetLimitOffsetByField(0, 1, ThreadRecordField::NumberID, std::to_string(numberID).c_str());

    // Thread not found, create one
    if (threadRec->size() == 0) {

        ThreadRecord re;
        re.numberID  = numberID;
        if (!threadInterface.Add(re)) {
            LOG_ERROR("Cannot create new thread");
            return false;
        }

        threadRec =
            threadInterface.GetLimitOffsetByField(0, 1, ThreadRecordField::NumberID, std::to_string(numberID).c_str());
        if (threadRec->size() == 0) {
            LOG_ERROR("Thread not found");
            return false;
        }
    }
    auto thread = (*threadRec)[0];

    // Create SMS
    if (!smsDB->sms.add(SMSTableRow{Record(DB_ID_NONE), // the entry is not yet in the DB
                                    .threadID  = thread.ID,
                                    .contactID = DB_ID_NONE,
                                    .date      = rec.date,
                                    .errorCode = rec.errorCode,
                                    .body      = rec.body,
                                    .type      = rec.type

        })) {
        LOG_ERROR("Cannot add sms");
        return false;
    }

    // TODO: error check

    // Update thread
    UpdateThreadSummary(thread, rec);

    thread.msgCount++;
    if (rec.type == SMSType::INBOX) {
        thread.unreadMsgCount++;
        LOG_DEBUG("unreadMsgCount = %" PRIu32 " for thread = %" PRIu32, thread.unreadMsgCount, thread.ID);
    }

    if (!threadInterface.Update(thread)) {
        LOG_ERROR("Cannot update thread");
        return false;
    }

    return true;
}
uint32_t SMSRecordInterface::GetCount()
{
    return smsDB->sms.count();
}

uint32_t SMSRecordInterface::GetLastID(void)
{
    return smsDB->getLastInsertRowId();
}
std::unique_ptr<std::vector<SMSRecord>> SMSRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                  uint32_t limit,
                                                                                  SMSRecordField field,
                                                                                  const char *str)
{
    auto records = std::make_unique<std::vector<SMSRecord>>();

    std::vector<SMSTableRow> smses;

    switch (field) {
    case SMSRecordField::ThreadID:
        smses = smsDB->sms.getLimitOffsetByField(offset, limit, SMSTableFields::ThreadID, str);
        break;
    default:
        LOG_ERROR("SMS thread get - wrong selection: %d", static_cast<int>(field));
        return records;
    }

    LOG_INFO("Get: %u SMS by selection: %d", static_cast<unsigned int>(smses.size()), static_cast<int>(field));

    ContactRecordInterface contactInterface(contactsDB);
    for (const auto &w : smses) {
        auto number = getPhoneNumberBySMS(w);
        if (number.has_value()) {
            records->push_back({w, number.value()});
        }
    }

    return records;
}

std::unique_ptr<std::vector<SMSRecord>> SMSRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto smses = smsDB->sms.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<SMSRecord>>();

    ContactRecordInterface contactInterface(contactsDB);
    for (const auto &w : smses) {
        auto number = getPhoneNumberBySMS(w);
        if (number.has_value()) {
            records->push_back({w, number.value()});
        }
    }

    return records;
}

bool SMSRecordInterface::Update(const SMSRecord &recUpdated)
{

    auto recCurrent = GetByID(recUpdated.ID);
    if (!recCurrent.isValid()) { // if updating non-existent entry
        return false;
    }

    smsDB->sms.update(SMSTableRow{Record(recCurrent.ID),
                                  .threadID  = recCurrent.threadID,
                                  .contactID = DB_ID_NONE,
                                  .date      = recUpdated.date,
                                  .errorCode = recUpdated.errorCode,
                                  .body      = recUpdated.body,
                                  .type      = recUpdated.type});

    // Update messages read count if necessary
    ThreadRecordInterface threadInterface(smsDB, contactsDB);
    auto thread = threadInterface.GetByID(recCurrent.threadID);

    // update thread details with the latest sms from given thread

    auto latest_vec = GetLimitOffsetByField(0, 1, SMSRecordField::ThreadID, std::to_string(thread.ID).c_str());

    if (latest_vec->size() == 0) {
        return false;
    }
    auto recLatestInThread = (*latest_vec)[0];

    // is updated sms is the latest we need to update the summary
    if (recUpdated.ID == recLatestInThread.ID) {
        // latest is visible, so update thread details
        UpdateThreadSummary(thread, recLatestInThread);
        threadInterface.Update(thread);
    }

    return true;
}

void SMSRecordInterface::UpdateThreadSummary(ThreadRecord &threadToUpdate, const SMSRecord &rec)
{
    threadToUpdate.snippet = rec.body.substr(
        0, std::min<size_t>({snippetLength, rec.body.length(), rec.body.find("\n"), rec.body.find("\r")}));
    threadToUpdate.date = rec.date;
    threadToUpdate.type = rec.type;
}

bool SMSRecordInterface::RemoveByID(uint32_t id)
{
    auto sms = smsDB->sms.getById(id);
    if (!sms.isValid()) {
        return false;
    }

    ThreadRecordInterface threadInterface(smsDB, contactsDB);
    auto threadRec = threadInterface.GetByID(sms.threadID);

    // If thread not found
    if (!threadRec.isValid()) {
        return smsDB->sms.removeById(id);
    }

    // If thread contains only one message remove it
    if (threadRec.msgCount <= 1) {
        threadInterface.RemoveByID(sms.threadID);
    }
    else {
        // update thread details if deleted SMS is the latest sms from the thread

        auto twoLatest = GetLimitOffsetByField(0, 2, SMSRecordField::ThreadID, std::to_string(threadRec.ID).c_str());

        if (twoLatest->size() == 0) {
            LOG_ERROR("Cannot fetch no SMSes even though thread's msgCount says so (id %lu)",
                      static_cast<long unsigned int>(sms.threadID));
            threadInterface.RemoveByID(sms.threadID);
            return false; // not handled (do no change window)
        }
        else {
            // check if there is need to change thread summary
            if (sms.ID == (*twoLatest)[0].ID) {
                // there is the need
                if (twoLatest->size() < 2) {
                    LOG_ERROR("Cannot fetch another sms from a thread even though thread's msgCount says so (id %lu)",
                              static_cast<long unsigned int>(sms.threadID));
                    smsDB->sms.removeById(id);                // remove sms which triggered this function
                    threadInterface.RemoveByID(sms.threadID); // and dangling thread as well
                    return true;                              // handled (current window will be changed)
                }
                else {
                    auto newLatest = (*twoLatest)[1];
                    UpdateThreadSummary(threadRec, newLatest);
                }
            }
            threadRec.msgCount--;
            threadInterface.Update(threadRec);
            // if deleting the newest sms, refresh thread details with next sms in the column
        }
    }

    // Remove SMS
    return smsDB->sms.removeById(id);
}

bool SMSRecordInterface::RemoveByField(SMSRecordField field, const char *str)
{
    switch (field) {
    case SMSRecordField::ThreadID:
        return smsDB->sms.removeByField(SMSTableFields::ThreadID, str);

    case SMSRecordField::Number:
        return false;

    default:
        return false;
    }
}

SMSRecord SMSRecordInterface::GetByID(uint32_t id)
{
    auto sms = smsDB->sms.getById(id);
    if (!sms.isValid()) {
        return {};
    }
    auto number = getPhoneNumberBySMS(sms);
    if (!number.has_value()) {
        return {};
    }
    return SMSRecord{sms, number.value()};
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::SMSSearchByType)) {
        return runQueryImpl(static_cast<const db::query::SMSSearchByType *>(query.get()));
    }
    else if (typeid(*query) == typeid(db::query::SMSGetByID)) {
        return getByIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetByThreadID)) {
        return getByThreadIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetByThreadIDWithTotalCount)) {
        return getByThreadIDQueryWithTotalCount(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetForList)) {
        return getForListQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetCountByThreadID)) {
        return getCountByThreadIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetByText)) {
        return getByTextQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetLastByThreadID)) {
        return getLastByThreadIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetCount)) {
        return getCountQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSAdd)) {
        return addQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSRemove)) {
        return removeQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSUpdate)) {
        return updateQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGet)) {
        return getQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSGetWithTotalCount)) {
        return getQueryWithTotalCount(query);
    }

    return nullptr;
}

std::unique_ptr<db::query::SMSSearchByTypeResult> SMSRecordInterface::runQueryImpl(
    const db::query::SMSSearchByType *query)
{
    auto db_result = smsDB->sms.getManyByType(query->type, query->starting_postion, query->depth);

    auto records = std::make_unique<std::vector<SMSRecord>>();

    ContactRecordInterface contactInterface(contactsDB);
    for (const auto &w : db_result.second) {
        auto number = getPhoneNumberBySMS(w);
        if (number.has_value()) {
            records->push_back(SMSRecord{w, number.value()});
        }
    }
    return std::make_unique<db::query::SMSSearchByTypeResult>(db_result.first, *records);
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getByIDQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSGetByID *>(query.get());
    auto sms              = smsDB->sms.getById(localQuery->id);

    auto response = std::make_unique<db::query::SMSGetByIDResult>(sms);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getByTextQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSGetByText *>(query.get());
    auto smsVector        = getByText(localQuery->getText(), localQuery->getPhoneNumber());
    auto recordVector     = std::vector<SMSRecord>(smsVector.begin(), smsVector.end());

    auto response = std::make_unique<db::query::SMSGetByTextResult>(recordVector);
    response->setRequestQuery(query);
    return response;
}

std::vector<SMSRecord> SMSRecordInterface::getByText(const std::string &text,
                                                     const std::optional<utils::PhoneNumber::View> &phoneNumberFilter)
{
    if (phoneNumberFilter.has_value()) {
        return getByTextAndPhoneNumber(text, phoneNumberFilter.value());
    }
    const auto &records = smsDB->sms.getByText(text);
    return std::vector<SMSRecord>(records.begin(), records.end());
}

std::vector<SMSRecord> SMSRecordInterface::getByTextAndPhoneNumber(const std::string &text,
                                                                   const utils::PhoneNumber::View &phoneNumber)
{
    ThreadRecordInterface threadsInterface{smsDB, contactsDB};
    const auto &thread = threadsInterface.GetByNumber(phoneNumber);
    if (!thread.isValid()) {
        return {};
    }

    const auto &smsRecords = smsDB->sms.getByText(text, thread.ID);
    return std::vector<SMSRecord>(smsRecords.begin(), smsRecords.end());
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getCountQuery(const std::shared_ptr<db::Query> &query)
{
    auto response = std::make_unique<db::query::SMSGetCountResult>(smsDB->sms.count());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getCountByThreadIDQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSGetCountByThreadID *>(query.get());
    auto response         = std::make_unique<db::query::SMSGetCountByThreadIDResult>(
        smsDB->sms.countByFieldId("thread_id", localQuery->threadId));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::addQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSAdd *>(query.get());
    auto record           = localQuery->record;
    const auto ret        = Add(record);
    if (ret) {
        record.ID = GetLastID();
    }
    auto response = std::make_unique<db::query::SMSAddResult>(record, ret);
    response->setRequestQuery(query);
    return response;
}
std::unique_ptr<db::QueryResult> SMSRecordInterface::removeQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSRemove *>(query.get());
    auto ret              = RemoveByID(localQuery->id);
    auto response         = std::make_unique<db::query::SMSRemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}
std::unique_ptr<db::QueryResult> SMSRecordInterface::updateQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSUpdate *>(query.get());
    auto ret              = Update(localQuery->record);
    auto response         = std::make_unique<db::query::SMSUpdateResult>(ret);
    response->setRequestQuery(query);
    return response;
}

auto SMSRecordInterface::getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<SMSRecord>
{
    const auto localQuery = static_cast<const db::query::SMSGet *>(query.get());
    auto smsVector        = smsDB->sms.getLimitOffset(localQuery->getOffset(), localQuery->getLimit());
    return std::vector<SMSRecord>(smsVector.begin(), smsVector.end());
}

auto SMSRecordInterface::getQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSGetResult>(getQueryRecords(query));
    response->setRequestQuery(query);
    return response;
}

auto SMSRecordInterface::getQueryWithTotalCount(const std::shared_ptr<db::Query> &query)
    -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSGetResultWithTotalCount>(getQueryRecords(query), smsDB->sms.count());
    response->setRequestQuery(query);
    return response;
}

auto SMSRecordInterface::getByThreadIDQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<SMSRecord>
{
    const auto localQuery = static_cast<const db::query::SMSGetByThreadID *>(query.get());
    auto smsVector        = smsDB->sms.getByThreadId(localQuery->threadId, localQuery->offset, localQuery->limit);
    return std::vector<SMSRecord>(smsVector.begin(), smsVector.end());
}

auto SMSRecordInterface::getByThreadIDQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSGetByThreadIDResult>(getByThreadIDQueryRecords(query));
    response->setRequestQuery(query);
    return response;
}

auto SMSRecordInterface::getByThreadIDQueryWithTotalCount(const std::shared_ptr<db::Query> &query)
    -> std::unique_ptr<db::QueryResult>
{
    const auto smsGetByThreadIDQuery = static_cast<const db::query::SMSGetByThreadID *>(query.get());
    auto response                    = std::make_unique<db::query::SMSGetByThreadIDResultWithTotalCount>(
        getByThreadIDQueryRecords(query), smsDB->sms.countWithoutDraftsByThreadId(smsGetByThreadIDQuery->threadId));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getForListQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSGetForList *>(query.get());

    auto number = ContactRecordInterface(contactsDB).GetNumberById(localQuery->numberID);
    auto smsVector =
        smsDB->sms.getByThreadIdWithoutDraftWithEmptyInput(localQuery->threadId, localQuery->offset, localQuery->limit);

    std::vector<SMSRecord> recordVector;
    std::transform(
        smsVector.begin(), smsVector.end(), std::back_inserter(recordVector), [number](const auto &recordRow) {
            return SMSRecord{recordRow, number};
        });
    auto count        = smsDB->sms.countWithoutDraftsByThreadId(localQuery->threadId);
    auto draft        = smsDB->sms.getDraftByThreadId(localQuery->threadId);

    auto response = std::make_unique<db::query::SMSGetForListResult>(recordVector, count, draft, number);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::getLastByThreadIDQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSGetLastByThreadID *>(query.get());
    const auto &result    = smsDB->sms.getByThreadId(localQuery->threadId, 0, 0);
    std::optional<SMSRecord> lastRecord;
    if (!result.empty()) {
        lastRecord = result.back();
    }

    auto response = std::make_unique<db::query::SMSGetLastByThreadIDResult>(lastRecord);
    response->setRequestQuery(query);
    return response;
}

auto SMSRecordInterface::getPhoneNumberBySMS(const SMSRecord &sms) -> std::optional<utils::PhoneNumber::View>
{
    auto thread = smsDB->threads.getById(sms.threadID);
    if (!thread.isValid()) {
        return {};
    }
    ContactRecordInterface contactInterface(contactsDB);
    return contactInterface.GetNumberById(thread.numberID);
}
