
/*
 * @file SMSRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "SMSRecord.hpp"
#include "ContactRecord.hpp"
#include "ThreadRecord.hpp"
#include <log/log.hpp>
#include <PhoneNumber.hpp>

#include <optional>

SMSRecord::SMSRecord(const SMSTableRow &w, const utils::PhoneNumber::View &num)
{
    ID        = w.ID;
    date      = w.date;
    dateSent  = w.dateSent;
    errorCode = w.errorCode;
    number    = num;
    body      = w.body;
    type      = w.type;
    threadID  = w.threadID;
    contactID = w.contactID;
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
    uint32_t threadID = 0;
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
    threadID = (*threadRec)[0].ID;

    // Create SMS
    if (!smsDB->sms.add(SMSTableRow{.threadID  = threadID,
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
    auto thread    = (*threadRec)[0];
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

bool SMSRecordInterface::Update(const SMSRecord &rec)
{

    auto sms = smsDB->sms.getById(rec.ID);
    if (sms.ID == 0) {
        return false;
    }

    return smsDB->sms.update(SMSTableRow{.ID        = rec.ID,
                                         .threadID  = sms.threadID,
                                         .contactID = sms.contactID,
                                         .date      = rec.date,
                                         .dateSent  = rec.dateSent,
                                         .errorCode = rec.errorCode,
                                         .body      = rec.body,
                                         .type      = rec.type});
}

bool SMSRecordInterface::RemoveByID(uint32_t id)
{

    auto sms = smsDB->sms.getById(id);
    if (sms.ID == 0) {
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
