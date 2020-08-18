#include "SMSRecord.hpp"
#include "Common/Query.hpp"
#include "ContactRecord.hpp"
#include "ThreadRecord.hpp"
#include "queries/sms/QuerySMSGetByContactID.hpp"
#include "queries/sms/QuerySMSGetByID.hpp"
#include "queries/sms/QuerySMSGetByText.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "queries/sms/QuerySMSSearchByType.hpp"
#include <log/log.hpp>

#include <PhoneNumber.hpp>
#include <optional>

SMSRecord::SMSRecord(const SMSTableRow &w, const utils::PhoneNumber::View &num)
    : date(w.date), dateSent(w.dateSent), errorCode(w.errorCode), body(w.body), type(w.type), threadID(w.threadID),
      contactID(w.contactID), number(num)
{
    this->ID = w.ID;
}

SMSRecordInterface::SMSRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb) : smsDB(smsDb), contactsDB(contactsDb)
{}

bool SMSRecordInterface::Add(const SMSRecord &rec)
{
    ContactRecordInterface contactInterface(contactsDB);
    auto contactRec = contactInterface.MatchByNumber(rec.number, ContactRecordInterface::CreateTempContact::True);
    if (contactRec == std::nullopt) {
        LOG_ERROR("Cannot find contact, for number %s", rec.number.getFormatted().c_str());
        return false;
    }
    uint32_t contactID = contactRec->ID;
    // Search for a thread with specified contactID
    ThreadRecordInterface threadInterface(smsDB, contactsDB);
    auto threadRec =
        threadInterface.GetLimitOffsetByField(0, 1, ThreadRecordField::ContactID, std::to_string(contactID).c_str());

    // Thread not found, create one
    if (threadRec->size() == 0) {

        ThreadRecord re;
        re.contactID = contactID;
        if (!threadInterface.Add(re)) {
            LOG_ERROR("Cannot create new thread");
            return false;
        }

        threadRec = threadInterface.GetLimitOffsetByField(
            0, 1, ThreadRecordField::ContactID, std::to_string(contactID).c_str());
        if (threadRec->size() == 0) {
            LOG_ERROR("Thread not found");
            return false;
        }
    }
    auto thread = (*threadRec)[0];

    // Create SMS
    if (!smsDB->sms.add(SMSTableRow{{.ID = DB_ID_NONE}, // the entry is not yet in the DB
                                    .threadID  = thread.ID,
                                    .contactID = contactID,
                                    .date      = rec.date,
                                    .dateSent  = rec.dateSent,
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
    case SMSRecordField ::ContactID:
        smses = smsDB->sms.getLimitOffsetByField(offset, limit, SMSTableFields::ContactID, str);
        break;

    case SMSRecordField ::ThreadID:
        smses = smsDB->sms.getLimitOffsetByField(offset, limit, SMSTableFields::ThreadID, str);
        break;

    default:
        LOG_ERROR("SMS thread get - wrong selection: %d", static_cast<int>(field));
        return records;
    }

    LOG_INFO("Get: %u SMS by selection: %d", static_cast<unsigned int>(smses.size()), static_cast<int>(field));

    ContactRecordInterface contactInterface(contactsDB);
    for (const auto &w : smses) {

        auto contactRec = contactInterface.GetByID(w.contactID);

        if (contactRec.numbers.size() != 0) {
            // TODO: or numberUser? or other number?
            records->push_back({w, contactRec.numbers[0].number});
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

        auto contactRec = contactInterface.GetByID(w.contactID);
        if (contactRec.numbers.size() != 0) {
            // TODO: or numberUser? or other number
            records->push_back({w, contactRec.numbers[0].number});
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

    smsDB->sms.update(SMSTableRow{{.ID = recCurrent.ID},
                                  .threadID  = recCurrent.threadID,
                                  .contactID = recCurrent.contactID,
                                  .date      = recUpdated.date,
                                  .dateSent  = recUpdated.dateSent,
                                  .errorCode = recUpdated.errorCode,
                                  .body      = recUpdated.body,
                                  .type      = recUpdated.type});

    // Update messages read count if necessary
    ThreadRecordInterface threadInterface(smsDB, contactsDB);
    auto thread = threadInterface.GetByID(recCurrent.threadID);

    // update thread details with the latest sms from given thread

    auto latest_vec = GetLimitOffsetByField(0, 1, SMSRecordField ::ThreadID, std::to_string(thread.ID).c_str());

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
    threadToUpdate.snippet = rec.body.substr(0, rec.body.length() >= snippetLength ? snippetLength : rec.body.length());
    threadToUpdate.date    = rec.date;
    threadToUpdate.type    = rec.type;
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

        auto twoLatest = GetLimitOffsetByField(0, 2, SMSRecordField ::ThreadID, std::to_string(threadRec.ID).c_str());

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
    if (smsDB->sms.removeById(id) == false) {
        return false;
    }

    return true;
}

bool SMSRecordInterface::RemoveByField(SMSRecordField field, const char *str)
{
    switch (field) {
    case SMSRecordField ::ContactID:
        return smsDB->sms.removeByField(SMSTableFields::ContactID, str);

    case SMSRecordField ::ThreadID:
        return smsDB->sms.removeByField(SMSTableFields::ThreadID, str);

    case SMSRecordField ::Number:
        return false;

    default:
        return false;
    }
}

SMSRecord SMSRecordInterface::GetByID(uint32_t id)
{
    auto sms = smsDB->sms.getById(id);

    ContactRecordInterface contactInterface(contactsDB);
    auto contactRec = contactInterface.GetByID(sms.contactID);
    // TODO: or numberUser?
    auto number = contactRec.numbers.size() != 0 ? contactRec.numbers[0].number : utils::PhoneNumber::View();

    return SMSRecord{sms, number};
}

std::unique_ptr<db::QueryResult> SMSRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (const auto localQuery = dynamic_cast<const db::query::SMSSearchByType *>(query.get())) {
        return runQueryImpl(localQuery);
    }
    else if (const auto localQuery = dynamic_cast<const db::query::SMSGetByID *>(query.get())) {
        auto sms = smsDB->sms.getById(localQuery->id);
        SMSRecord record;

        record.body      = sms.body;
        record.contactID = sms.contactID;
        record.date      = sms.date;
        record.dateSent  = sms.dateSent;
        record.errorCode = sms.errorCode;
        record.threadID  = sms.threadID;
        record.type      = sms.type;
        record.ID        = sms.ID;
        auto response    = std::make_unique<db::query::SMSGetByIDResult>(record);
        response->setRequestQuery(query);
        return response;
    }
    else if (const auto localQuery = dynamic_cast<const db::query::SMSGetByContactID *>(query.get())) {
        auto smsVector = smsDB->sms.getByContactId(localQuery->contactId);
        std::vector<SMSRecord> recordVector;
        for (auto sms : smsVector) {
            SMSRecord record;
            record.body      = sms.body;
            record.contactID = sms.contactID;
            record.date      = sms.date;
            record.dateSent  = sms.dateSent;
            record.errorCode = sms.errorCode;
            record.threadID  = sms.threadID;
            record.type      = sms.type;
            record.ID        = sms.ID;
            recordVector.emplace_back(record);
        }

        auto response = std::make_unique<db::query::SMSGetByContactIDResult>(recordVector);
        response->setRequestQuery(query);
        return response;
    }
    else if (const auto localQuery = dynamic_cast<const db::query::SMSGetByText *>(query.get())) {
        auto smsVector = smsDB->sms.getByText(localQuery->text);
        std::vector<SMSRecord> recordVector;
        for (auto sms : smsVector) {
            SMSRecord record;
            record.body      = sms.body;
            record.contactID = sms.contactID;
            record.date      = sms.date;
            record.dateSent  = sms.dateSent;
            record.errorCode = sms.errorCode;
            record.threadID  = sms.threadID;
            record.type      = sms.type;
            record.ID        = sms.ID;
            recordVector.emplace_back(record);
        }

        auto response = std::make_unique<db::query::SMSGetByTextResult>(recordVector);
        response->setRequestQuery(query);
        return response;
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

        auto contactRec = contactInterface.GetByID(w.contactID);
        if (contactRec.numbers.size() != 0) {
            // TODO: or numberUser? or other number
            records->push_back({w, contactRec.numbers[0].number});
        }
    }
    return std::make_unique<db::query::SMSSearchByTypeResult>(db_result.first, *records);
}
