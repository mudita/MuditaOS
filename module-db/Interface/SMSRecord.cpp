#include "SMSRecord.hpp"
#include "ContactRecord.hpp"
#include "ThreadRecord.hpp"
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
    thread.snippet = rec.body.substr(0, rec.body.length() >= snippetLength ? snippetLength : rec.body.length());
    thread.date    = rec.date;
    thread.type    = rec.type;
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

    smsDB->sms.update(SMSTableRow{{.ID = recUpdated.ID},
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
    auto latest =
        smsDB->sms.getLimitOffsetByField(0, 1, SMSTableFields::ThreadID, std::to_string(thread.dbID).c_str())[0];
    // check if there is need to change thread summary
    if (latest.ID == recCurrent.ID) { // there is no == operator overloaded
        thread.snippet = recUpdated.body.substr(0, recUpdated.body.length() >= snippetLength ? snippetLength : rec.body.length());
        thread.date    = recUpdated.date;
        thread.type    = recUpdated.type;
    }

    threadInterface.Update(thread);

    return true;
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
        if (smsDB->sms.removeById(id) == false) {
            return false;
        }

        return false;
    }

    // If thread contains only one message remove it
    if (threadRec.msgCount == 1) {
        threadInterface.RemoveByID(sms.threadID);
    }
    else {
        // update thread details with the latest sms from given thread
        auto twoLatest =
            smsDB->sms.GetLimitOffsetByField(0, 2, SMSTableFields::ThreadID, std::to_string(threadRec.dbID).c_str());
        // check if there is need to change thread summary
        if (twoLatest[0].ID == sms.ID) { // there is no == operator overloaded
            // if deleting the newest sms, refresh thread details with next sms in the column
            auto newLatest    = twoLatest[1];
            threadRec.snippet = newLatest.body.substr(
                0, newLatest.body.length() >= snippetLength ? snippetLength : newLatest.body.length());
            threadRec.date = newLatest.date;
            threadRec.type = newLatest.type;
        }
        // Update msg count
        threadRec.msgCount--;
        threadInterface.Update(threadRec);
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

std::unique_ptr<db::QueryResult> SMSRecordInterface::runQuery(const db::Query *query)
{
    if (const auto local_query = dynamic_cast<const db::query::SMSSearchByType *>(query)) {
        return runQueryImpl(local_query);
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
