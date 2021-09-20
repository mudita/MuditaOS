// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogRecord.hpp"

#include <ContactRecord.hpp>
#include <log.hpp>
#include <Tables/CalllogTable.hpp>
#include <PhoneNumber.hpp>
#include <Utils.hpp>
#include "queries/calllog/QueryCalllogGet.hpp"
#include "queries/calllog/QueryCalllogGetCount.hpp"
#include "queries/calllog/QueryCalllogRemove.hpp"
#include "queries/calllog/QueryCalllogGetByContactID.hpp"

#include <cassert>
#include <exception>
#include <sstream>
#include <vector>
#include <utility>

CalllogRecord::CalllogRecord(const CalllogTableRow &tableRow)
    : Record{tableRow.ID}, presentation(tableRow.presentation), date(tableRow.date), duration(tableRow.duration),
      type(tableRow.type), name(tableRow.name), contactId(tableRow.contactId),
      phoneNumber(utils::PhoneNumber(tableRow.number, tableRow.e164number).getView()), isRead(tableRow.isRead)
{}

CalllogRecord::CalllogRecord(const CallType type, const utils::PhoneNumber::View &number)
    : presentation(PresentationType::PR_UNKNOWN), date(std::time(nullptr)), type(type), phoneNumber(number)
{}

uint32_t CalllogRecord::getContactId() const
{
    return contactId;
}

std::ostream &operator<<(std::ostream &out, const CalllogRecord &rec)
{
    out << " <id> " << rec.ID << " <number> " << rec.phoneNumber.getEntered() << " <e164number> "
        << rec.phoneNumber.getE164() << " <formatted> " << rec.phoneNumber.getFormatted() << " <presentation> "
        << static_cast<uint32_t>(rec.presentation) << " <date> " << rec.date << " <duration> " << rec.duration
        << " <type> " << static_cast<uint32_t>(rec.type) << " <name> " << rec.name << " <contactID> " << rec.contactId
        << " <isRead> " << rec.isRead;

    return out;
}

[[nodiscard]] std::string CalllogRecord::str() const
{
    std::ostringstream ss;
    ss << *this;
    return ss.str();
}

CalllogRecordInterface::CalllogRecordInterface(CalllogDB *calllogDb, ContactsDB *contactsDb)
    : calllogDB(calllogDb), contactsDB(contactsDb)
{}

bool CalllogRecordInterface::Add(const CalllogRecord &rec)
{
    auto localRec = rec;
    if (!rec.phoneNumber.getFormatted().empty()) {
        ContactRecordInterface contactInterface(contactsDB);
        auto contactMatch =
            contactInterface.MatchByNumber(rec.phoneNumber, ContactRecordInterface::CreateTempContact::True);
        if (!contactMatch) {
            LOG_ERROR("Cannot get contact, for id %" PRIu32, rec.contactId);
            return false;
        }
        auto &contactRec = contactMatch->contact;

        localRec.contactId = contactRec.ID;
        localRec.name      = contactRec.getFormattedName();
        if (localRec.presentation == PresentationType::PR_UNKNOWN) {
            localRec.presentation = PresentationType::PR_ALLOWED;
        }
        LOG_DEBUG("Adding call log record");
    }
    else {
        // private number so do not add contact just call log entry
        localRec.presentation = PresentationType::PR_UNKNOWN;
        LOG_DEBUG("Adding private call entry to call log record.");
    }

    auto r         = CalllogTableRow{};
    r.ID           = localRec.ID;
    r.number       = localRec.phoneNumber.getEntered();
    r.e164number   = localRec.phoneNumber.getE164();
    r.presentation = localRec.presentation;
    r.date         = localRec.date;
    r.duration     = localRec.duration;
    r.type         = localRec.type;
    r.name         = localRec.name;
    r.contactId    = localRec.contactId;
    r.isRead       = localRec.isRead;
    return calllogDB->calls.add(r);
}

uint32_t CalllogRecordInterface::GetLastID()
{
    return calllogDB->getLastInsertRowId();
}

std::unique_ptr<std::vector<CalllogRecord>> CalllogRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                          uint32_t limit,
                                                                                          CalllogRecordField field,
                                                                                          const char *str)
{
    // TODO: alek: need proper implementation
    return GetLimitOffset(offset, limit);
}

ContactRecord CalllogRecordInterface::GetContactRecordByID(uint32_t contactId)
{
    assert(contactsDB != nullptr);
    ContactRecordInterface contactInterface(contactsDB);
    return contactInterface.GetByIdWithTemporary(contactId);
}

std::unique_ptr<std::vector<CalllogRecord>> CalllogRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto calls = calllogDB->calls.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<CalllogRecord>>();
    LOG_DEBUG("getting callog records");
    for (auto &c : calls) {
        LOG_DEBUG(" - call id: %" PRIu32 "| contactId: %" PRIu32, c.ID, c.contactId);
        auto contactRec = GetContactRecordByID(c.contactId);
        if (!contactRec.isValid()) {
            LOG_ERROR("Cannot find contact for ID %" PRIu32 " (id:%" PRIu32 ")", c.contactId, c.ID);
            c.name = c.number;
        }
        else {
            c.name = contactRec.getFormattedName();
        }
        records->push_back(c);
    }

    return records;
}

bool CalllogRecordInterface::Update(const CalllogRecord &rec)
{
    auto call = calllogDB->calls.getById(rec.ID);
    if (!call.isValid()) {
        return false;
    }

    auto contactID    = rec.contactId;
    auto presentation = rec.presentation;
    if (call.presentation == PresentationType::PR_UNKNOWN && rec.phoneNumber.isValid()) {
        // entry added as private was updated with phone number
        ContactRecordInterface contactInterface(contactsDB);
        auto contactMatch =
            contactInterface.MatchByNumber(rec.phoneNumber, ContactRecordInterface::CreateTempContact::True);
        if (!contactMatch) {
            LOG_ERROR("Cannot get or create temporary contact for id %" PRIu32, rec.contactId);
            return false;
        }
        contactID = contactMatch->contact.ID;
        if (presentation == PresentationType::PR_UNKNOWN) {
            presentation = PresentationType::PR_ALLOWED;
        }
    }

    auto r         = CalllogTableRow{};
    r.ID           = rec.ID;
    r.number       = rec.phoneNumber.getEntered();
    r.e164number   = rec.phoneNumber.getE164();
    r.presentation = presentation;
    r.date         = rec.date;
    r.duration     = rec.duration;
    r.type         = rec.type;
    r.name         = rec.name;
    r.contactId    = contactID;
    r.isRead       = rec.isRead;
    return calllogDB->calls.update(r);
}

bool CalllogRecordInterface::RemoveByID(uint32_t id)
{

    auto call = calllogDB->calls.getById(id);
    if (call.ID == 0) {
        return false;
    }

    return calllogDB->calls.removeById(id);
}

bool CalllogRecordInterface::RemoveByField(CalllogRecordField field, const char *str)
{

    switch (field) {
    case CalllogRecordField::DATE:
        return calllogDB->calls.removeByField(CalllogTableFields::DATE, str);
    case CalllogRecordField::TYPE:
        return calllogDB->calls.removeByField(CalllogTableFields::TYPE, str);
    default:
        return false;
    }
}

CalllogRecord CalllogRecordInterface::GetByID(uint32_t id)
{
    auto call = calllogDB->calls.getById(id);

    if (call.contactId != DB_ID_NONE) {
        auto contactRec = GetContactRecordByID(call.contactId);
        if (contactRec.ID == DB_ID_NONE) {
            LOG_ERROR("Cannot find contact for ID %" PRIu32, call.contactId);
            return CalllogRecord();
        }

        call.name = contactRec.getFormattedName();
    }

    return CalllogRecord(call);
}

std::vector<CalllogRecord> CalllogRecordInterface::GetByContactID(uint32_t id)
{
    auto calls = calllogDB->calls.getByContactId(id);
    std::vector<CalllogRecord> records;
    for (auto &call : calls) {
        auto contactRec = GetContactRecordByID(call.contactId);
        if (contactRec.ID == DB_ID_NONE) {
            LOG_ERROR("Cannot find contact for ID %" PRIu32, call.contactId);
            records.emplace_back(CalllogRecord());
        }

        call.name = contactRec.getFormattedName();
        records.emplace_back(CalllogRecord(call));
    }

    return records;
}

uint32_t CalllogRecordInterface::GetCount(EntryState state)
{
    return calllogDB->calls.count(state);
}

uint32_t CalllogRecordInterface::GetCount()
{
    return GetCount(EntryState::ALL);
}

bool CalllogRecordInterface::SetAllRead()
{
    return calllogDB->calls.SetAllRead();
}

std::unique_ptr<db::QueryResult> CalllogRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::CalllogGet)) {
        return getQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::CalllogGetByContactID)) {
        return getByContactIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::calllog::SetAllRead)) {
        return setAllReadQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::CalllogGetCount)) {
        return getCountQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::CalllogRemove)) {
        return removeQuery(query);
    }
    return nullptr;
}
std::unique_ptr<db::QueryResult> CalllogRecordInterface::getQuery(std::shared_ptr<db::Query> query)
{
    auto getQuery = static_cast<db::query::CalllogGet *>(query.get());
    auto records  = calllogDB->calls.getLimitOffset(getQuery->getOffset(), getQuery->getLimit());
    std::vector<CalllogRecord> recordVector;

    for (auto calllog : records) {
        CalllogRecord record;
        record.isRead       = calllog.isRead;
        record.phoneNumber  = utils::PhoneNumber::parse(calllog.number);
        record.contactId    = calllog.contactId;
        record.name         = calllog.name;
        record.type         = calllog.type;
        record.duration     = calllog.duration;
        record.date         = calllog.date;
        record.presentation = calllog.presentation;
        record.ID           = calllog.ID;
        recordVector.emplace_back(record);
    }

    auto response = std::make_unique<db::query::CalllogGetResult>(std::move(recordVector), GetCount());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> CalllogRecordInterface::getByContactIDQuery(std::shared_ptr<db::Query> query)
{
    auto getQuery = static_cast<db::query::CalllogGetByContactID *>(query.get());
    auto records  = CalllogRecordInterface::GetByContactID(getQuery->contactId);

    auto response = std::make_unique<db::query::CalllogGetByContactIDResult>(std::move(records));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> CalllogRecordInterface::setAllReadQuery(std::shared_ptr<db::Query> query)
{
    auto db_result = SetAllRead();
    auto response  = std::make_unique<db::query::calllog::SetAllReadResult>(db_result);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> CalllogRecordInterface::getCountQuery(std::shared_ptr<db::Query> query)
{
    auto localQuery = static_cast<db::query::CalllogGetCount *>(query.get());
    auto count      = GetCount(localQuery->getState());
    auto response   = std::make_unique<db::query::CalllogGetCountResult>(localQuery->getState(), count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> CalllogRecordInterface::removeQuery(std::shared_ptr<db::Query> query)
{
    auto removeQuery = static_cast<db::query::CalllogRemove *>(query.get());
    auto ret         = CalllogRecordInterface::RemoveByID(removeQuery->id);
    auto response    = std::make_unique<db::query::CalllogRemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}
