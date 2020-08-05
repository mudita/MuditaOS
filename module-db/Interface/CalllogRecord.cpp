/*
 * @file CalllogRecord.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Record
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogRecord.hpp"

#include <ContactRecord.hpp>
#include <log/log.hpp>
#include <Tables/CalllogTable.hpp>
#include <PhoneNumber.hpp>
#include <Utils.hpp>

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

uint32_t CalllogRecord::getContactId() const
{
    try {
        return static_cast<uint32_t>(std::stoi(contactId));
    }
    catch (const std::exception &e) {
        LOG_ERROR("Invalid contactId %s", contactId.c_str());
        return DB_ID_NONE;
    }
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

CalllogRecordInterface::CalllogRecordInterface(CalllogDB *calllogDb, ContactsDB *contactsDb)
    : calllogDB(calllogDb), contactsDB(contactsDb)
{}

CalllogRecordInterface::~CalllogRecordInterface()
{}

bool CalllogRecordInterface::Add(const CalllogRecord &rec)
{
    ContactRecordInterface contactInterface(contactsDB);
    auto contactRec = contactInterface.MatchByNumber(rec.phoneNumber, ContactRecordInterface::CreateTempContact::True);
    if (!contactRec) {
        LOG_ERROR("Cannot get contact, for number %s", rec.phoneNumber.getNonEmpty().c_str());
        return false;
    }
    auto localRec      = rec;
    localRec.contactId = std::to_string(contactRec->ID);
    localRec.name      = contactRec->getFormattedName();
    LOG_DEBUG("Adding calllog record %s", utils::to_string(localRec).c_str());

    return calllogDB->calls.add(CalllogTableRow{{.ID = localRec.ID}, // this is only to remove warning
                                                .number       = localRec.phoneNumber.getEntered(),
                                                .e164number   = localRec.phoneNumber.getE164(),
                                                .presentation = localRec.presentation,
                                                .date         = localRec.date,
                                                .duration     = localRec.duration,
                                                .type         = localRec.type,
                                                .name         = localRec.name,
                                                .contactId    = localRec.contactId,
                                                .isRead       = localRec.isRead});
    ;
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

ContactRecord CalllogRecordInterface::GetContactRecordByID(const UTF8 &contactId)
{
    assert(contactsDB != nullptr);
    ContactRecordInterface contactInterface(contactsDB);
    try {
        return contactInterface.GetByID(std::atoi(contactId.c_str()));
    }
    catch (const std::exception &e) {
        LOG_ERROR("Exception %s occured", e.what());
        return ContactRecord();
    }
}

std::unique_ptr<std::vector<CalllogRecord>> CalllogRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto calls = calllogDB->calls.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<CalllogRecord>>();

    for (auto &c : calls) {
        auto contactRec = GetContactRecordByID(c.contactId);
        if (!contactRec.isValid()) {
            LOG_ERROR("Cannot find contact for ID %s", c.contactId.c_str());
            continue;
        }

        c.name = contactRec.getFormattedName();
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

    return calllogDB->calls.update(CalllogTableRow{{.ID = rec.ID},
                                                   .number       = rec.phoneNumber.getEntered(),
                                                   .e164number   = rec.phoneNumber.getE164(),
                                                   .presentation = rec.presentation,
                                                   .date         = rec.date,
                                                   .duration     = rec.duration,
                                                   .type         = rec.type,
                                                   .name         = rec.name,
                                                   .contactId    = rec.contactId,
                                                   .isRead       = rec.isRead});
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

    auto contactRec = GetContactRecordByID(call.contactId);
    if (contactRec.ID == DB_ID_NONE) {
        auto contactRec = GetContactRecordByID(call.contactId);
        LOG_ERROR("Cannot find contact for ID %s", call.contactId.c_str());
        return CalllogRecord();
    }

    call.name = contactRec.getFormattedName();
    return CalllogRecord(call);
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
    if (const auto local_query = dynamic_cast<const db::query::calllog::SetAllRead *>(query.get())) {
        return runQueryImpl(local_query);
    }
    return nullptr;
}

std::unique_ptr<db::query::calllog::SetAllReadResult> CalllogRecordInterface::runQueryImpl(
    const db::query::calllog::SetAllRead *query)
{
    auto db_result = SetAllRead();
    return std::make_unique<db::query::calllog::SetAllReadResult>(db_result);
}
