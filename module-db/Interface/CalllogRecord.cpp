/*
 * @file CalllogRecord.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Record
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogRecord.hpp"
#include "../Tables/CalllogTable.hpp"
#include "ContactRecord.hpp"
#include <Utils.hpp>
#include <cassert>
#include <segger/log/log.hpp>
#include <sstream>

CalllogRecord::CalllogRecord(const CalllogTableRow &tableRow, const UTF8 &num, const UTF8 &name)
    : Record{tableRow.id}, number(num), presentation(tableRow.presentation), date(tableRow.date),
      duration(tableRow.duration), type(tableRow.type), name(name), contactId(tableRow.contactId)
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
    out << " <id> " << rec.ID << " <number> " << rec.number << " <presentation> "
        << static_cast<uint32_t>(rec.presentation) << " <date> " << rec.date << " <duration> " << rec.duration
        << " <type> " << static_cast<uint32_t>(rec.type) << " <name> " << rec.name << " <contactID> " << rec.contactId;

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
    auto contactRec = contactInterface.GetByNumber(rec.number, ContactRecordInterface::CreateTempContact::True);
    if (contactRec->size() == 0) {
        LOG_ERROR("Cannot get contact, for number %s", rec.number.c_str());
        return false;
    }
    auto localRec      = rec;
    auto contact       = (*contactRec)[0];
    localRec.contactId = std::to_string(contact.ID);
    localRec.name      = contact.getFormattedName();
    LOG_DEBUG("Adding calllog record %s", utils::to_string(localRec).c_str());

    return calllogDB->calls.Add(CalllogTableRow{.id           = localRec.ID, // this is only to remove warning
                                                .number       = localRec.number,
                                                .presentation = localRec.presentation,
                                                .date         = localRec.date,
                                                .duration     = localRec.duration,
                                                .type         = localRec.type,
                                                .name         = localRec.name,
                                                .contactId    = localRec.contactId});
    ;
}

uint32_t CalllogRecordInterface::GetLastID()
{
    return calllogDB->GetLastInsertRowID();
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
    auto calls = calllogDB->calls.GetLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<CalllogRecord>>();

    for (const auto &c : calls) {
        auto contactRec = GetContactRecordByID(c.contactId);
        if (contactRec.ID == DB_ID_NONE) {
            LOG_ERROR("Cannot find contact for ID %s", c.contactId.c_str());
            continue;
        }

        records->push_back({c, contactRec.numbers[0].numberE164, contactRec.getFormattedName()});
    }

    return records;
}

bool CalllogRecordInterface::Update(const CalllogRecord &rec)
{

    auto call = calllogDB->calls.GetByID(rec.ID);
    if (call.id == 0) {
        return false;
    }

    return calllogDB->calls.Update(CalllogTableRow{.id           = rec.ID,
                                                   .number       = rec.number,
                                                   .presentation = rec.presentation,
                                                   .date         = rec.date,
                                                   .duration     = rec.duration,
                                                   .type         = rec.type,
                                                   .name         = rec.name,
                                                   .contactId    = rec.contactId});
}

bool CalllogRecordInterface::RemoveByID(uint32_t id)
{

    auto call = calllogDB->calls.GetByID(id);
    if (call.id == 0) {
        return false;
    }

    return calllogDB->calls.RemoveByID(id);
}

bool CalllogRecordInterface::RemoveByField(CalllogRecordField field, const char *str)
{

    switch (field) {
    case CalllogRecordField::DATE:
        return calllogDB->calls.RemoveByField(CalllogTableFields::DATE, str);
    case CalllogRecordField::TYPE:
        return calllogDB->calls.RemoveByField(CalllogTableFields::TYPE, str);
    default:
        return false;
    }
}

CalllogRecord CalllogRecordInterface::GetByID(uint32_t id)
{
    auto call = calllogDB->calls.GetByID(id);

    auto contactRec = GetContactRecordByID(call.contactId);
    if (contactRec.ID == DB_ID_NONE) {
        auto contactRec = GetContactRecordByID(call.contactId);
        LOG_ERROR("Cannot find contact for ID %s", call.contactId.c_str());
        return CalllogRecord();
    }

    return CalllogRecord{call, contactRec.numbers[0].numberE164, contactRec.getFormattedName()};
}

uint32_t CalllogRecordInterface::GetCount(EntryState state)
{
    return calllogDB->calls.GetCount(state);
}

uint32_t CalllogRecordInterface::GetCount()
{
    return GetCount(EntryState::ALL);
}
