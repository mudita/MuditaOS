// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactRecord.hpp"
#include "queries/phonebook/QueryContactAdd.hpp"
#include "queries/phonebook/QueryContactGetByID.hpp"
#include "queries/phonebook/QueryContactUpdate.hpp"
#include "queries/phonebook/QueryContactRemove.hpp"
#include <Utils.hpp>

#include <queries/phonebook/QueryContactGet.hpp>
#include <queries/phonebook/QueryNumberGetByID.hpp>

#include <PhoneNumber.hpp>
#include <NumberHolderMatcher.hpp>

#include <algorithm>
#include <iterator>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

ContactRecordInterface::ContactRecordInterface(ContactsDB *db)
    : contactDB(db), favouritesGroupId(db->groups.favouritesId())
{}

auto ContactRecordInterface::Add(ContactRecord &rec) -> bool
{
    bool ret = contactDB->contacts.add(
        ContactsTableRow{{.ID = DB_ID_NONE}, .speedDial = rec.speeddial, .namePrimary = rec.primaryName});

    if (!ret) {
        return false;
    }

    uint32_t contactID = contactDB->getLastInsertRowId();
    debug_db_data("New contact with ID %" PRIu32 " created", contactID);
    rec.ID = contactID;

    ret = contactDB->name.add(ContactsNameTableRow{{.ID = DB_ID_NONE},
                                                   .contactID       = contactID,
                                                   .namePrimary     = rec.primaryName,
                                                   .nameAlternative = rec.alternativeName});

    if (!ret) {
        return false;
    }

    auto contactNameID = contactDB->getLastInsertRowId();

    // build string list of number IDs
    std::string numbersIDs;
    for (auto a : rec.numbers) {
        ret = contactDB->number.add(ContactsNumberTableRow{{.ID = DB_ID_NONE},
                                                           .contactID  = contactID,
                                                           .numberUser = a.number.getEntered().c_str(),
                                                           .numbere164 = a.number.getE164().c_str(),
                                                           .type       = a.numberType});
        if (!ret) {
            return false;
        }

        numbersIDs += std::to_string(contactDB->getLastInsertRowId()) + " ";
    }

    // remove trailing space
    if (!numbersIDs.empty()) {
        numbersIDs.pop_back();
    }

    ret = contactDB->ringtones.add(ContactsRingtonesTableRow(DB_ID_NONE, contactID, rec.assetPath));

    if (!ret) {
        return false;
    }

    auto contactRingID = contactDB->getLastInsertRowId();

    ret = contactDB->address.add(ContactsAddressTableRow{
        {.ID = DB_ID_NONE}, .contactID = contactID, .address = rec.address, .note = rec.note, .mail = rec.mail});

    if (!ret) {
        return false;
    }

    auto contactAddressID = contactDB->getLastInsertRowId();

    ret = contactDB->contacts.update(ContactsTableRow{{.ID = contactID},
                                                      .nameID    = contactNameID,
                                                      .numbersID = numbersIDs,
                                                      .ringID    = contactRingID,
                                                      .addressID = contactAddressID,
                                                      .speedDial = rec.speeddial});
    for (auto group : rec.groups) {
        contactDB->groups.addContactToGroup(contactID, group.ID);
    }

    return ret;
}

auto ContactRecordInterface::BlockByID(uint32_t id, const bool shouldBeBlocked) -> bool
{
    return contactDB->contacts.BlockByID(id, shouldBeBlocked);
}

auto ContactRecordInterface::RemoveByID(uint32_t id) -> bool
{
    auto contact = contactDB->contacts.getByIdWithTemporary(id);
    if (contact.isValid()) {
        auto currentGroups = contactDB->groups.getGroupsForContact(id);
        if (currentGroups.find(ContactsDB::temporaryGroupId()) == currentGroups.end()) {
            for (auto group : currentGroups) {
                contactDB->groups.removeContactFromGroup(id, group.ID);
            }
            contact.speedDial = "";
            //            contactDB->name.removeById(contact.nameID);
            contactDB->address.removeById(contact.addressID);
            contactDB->ringtones.removeById(contact.ringID);
            contactDB->contacts.update(contact);
            contactDB->groups.addContactToGroup(id, ContactsDB::temporaryGroupId());
        }
        // already deleted
    }
    return true;
}

auto ContactRecordInterface::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    if (query->type != db::Query::Type::Read) {
        debug_db_data("Received unhandled query type: %lu", static_cast<unsigned long int>(query->type));
        return nullptr;
    }

    if (typeid(*query) == typeid(db::query::ContactGet)) {
        return getQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetLetterMap)) {
        return getLetterMapQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetByID)) {
        return getByIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetSize)) {
        return getSizeQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactAdd)) {
        return addQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactUpdate)) {
        return updateQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactRemove)) {
        return removeQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::NumberGetByID)) {
        return numberGetByIdQuery(query);
    }
    else {
        error_db_data("Unexpected query type.");
        return nullptr;
    }
}

auto ContactRecordInterface::getQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto textFilter = dynamic_cast<const db::query::TextFilter *>(query.get());
    assert(query != nullptr);
    bool searchByNumber = false;
    if (textFilter != nullptr && textFilter->isFilterPresent() && utils::is_number(textFilter->getFilterData())) {
        searchByNumber = true;
        debug_db_data("Filtering by number: %s", textFilter->getFilterData().c_str());
    }

    auto readQuery = static_cast<const db::query::ContactGet *>(query.get());
    debug_db_data("Contact read query, filter: \"%s\", offset=%lu, limit=%lu",
                  readQuery->getFilterData().c_str(),
                  static_cast<unsigned long>(readQuery->getOffset()),
                  static_cast<unsigned long>(readQuery->getLimit()));
    auto [limit, offset] = readQuery->getLimitOffset();
    auto matchType       = searchByNumber ? ContactsTable::MatchType::TextNumber : ContactsTable::MatchType::Name;
    uint32_t groupID     = readQuery->getGroupFilterData();
    if (groupID != DB_ID_NONE) {
        matchType = ContactsTable::MatchType::Group;
    }
    else {
        groupID = favouritesGroupId;
    }
    debug_db_data("Contact match Type: %lu", static_cast<unsigned long int>(matchType));

    std::vector<std::uint32_t> ids;

    if (readQuery->getContactDisplayMode() == static_cast<uint32_t>(ContactDisplayMode::Regular)) {
        ids = contactDB->contacts.GetIDsSortedByField(matchType, readQuery->getFilterData(), groupID, limit, offset);
    }
    else {
        ids = contactDB->contacts.GetIDsSortedByName(limit, offset);
    }
    debug_db_data("Received records: %lu", static_cast<unsigned long int>(ids.size()));

    std::vector<ContactRecord> result(ids.size());
    std::transform(std::begin(ids), std::end(ids), std::begin(result), [this](uint32_t id) { return GetByID(id); });
    for (uint32_t idx = 0; idx < static_cast<uint32_t>(ids.size()); idx++) {
        result[idx].contactPosOnList = offset + idx;
    }
    auto response = std::make_unique<db::query::ContactGetResult>(result);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getLetterMapQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    ContactsMapData result = contactDB->contacts.GetPosOfFirstLetters();
    auto response          = std::make_unique<db::query::ContactGetLetterMapResult>(result);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getByIDQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto readQuery = static_cast<db::query::ContactGetByID *>(query.get());
    ContactRecord record;

    if (readQuery->getWithTemporary()) {
        record = ContactRecordInterface::GetByIdWithTemporary(readQuery->getID());
    }
    else {
        record = ContactRecordInterface::GetByID(readQuery->getID());
    }

    auto response = std::make_unique<db::query::ContactGetByIDResult>(record);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getSizeQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto textFilter = dynamic_cast<const db::query::TextFilter *>(query.get());
    assert(query != nullptr);
    bool searchByNumber = false;
    if (textFilter != nullptr && textFilter->isFilterPresent() && utils::is_number(textFilter->getFilterData())) {
        searchByNumber = true;
        debug_db_data("Filtering by number: %s", textFilter->getFilterData().c_str());
    }

    auto countQuery = static_cast<const db::query::ContactGetSize *>(query.get());
    debug_db_data("Contact count query, filter: \"%s\"", countQuery->getFilterData().c_str());

    std::size_t count = 0;
    if (!countQuery->isFilterPresent()) {
        uint32_t groupID = countQuery->getGroupFilterData();
        if (groupID != DB_ID_NONE) {
            count = contactDB->contacts
                        .GetIDsSortedByField(ContactsTable::MatchType::Group, countQuery->getFilterData(), groupID)
                        .size();
        }
        else {
            if (countQuery->getContactDisplayMode() == static_cast<uint32_t>(ContactDisplayMode::Regular)) {
                count = contactDB->contacts.count();
            }
            else {
                count = contactDB->contacts.GetIDsSortedByName().size();
            }
        }
    }
    else if (searchByNumber) {

        uint32_t groupID = countQuery->getGroupFilterData();
        // incl search by letter
        if (groupID != DB_ID_NONE) {
            count = contactDB->contacts
                        .GetIDsSortedByField(ContactsTable::MatchType::Group, countQuery->getFilterData(), groupID)
                        .size();
        }
        else {
            count = contactDB->contacts.count();
        }
    }

    else {
        count = contactDB->name.GetCountByName(countQuery->getFilterData());
    }

    debug_db_data("Contact count query result: %lu", static_cast<unsigned long>(count));

    auto response = std::make_unique<db::query::RecordsSizeQueryResult>(count);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getForListQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    return nullptr;
}

auto ContactRecordInterface::addQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto addQuery = dynamic_cast<db::query::ContactAdd *>(query.get());
    auto ret      = ContactRecordInterface::Add(addQuery->rec);
    auto response = std::make_unique<db::query::ContactAddResult>(ret, addQuery->rec.ID);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::updateQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto updateQuery = static_cast<const db::query::ContactUpdate *>(query.get());
    auto ret         = ContactRecordInterface::Update(updateQuery->rec);
    auto response    = std::make_unique<db::query::ContactUpdateResult>(ret);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::removeQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto removeQuery = static_cast<db::query::ContactRemove *>(query.get());
    auto ret         = ContactRecordInterface::RemoveByID(removeQuery->getID());
    auto response    = std::make_unique<db::query::ContactRemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::numberGetByIdQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    auto numberQuery = static_cast<db::query::NumberGetByID *>(query.get());
    auto ret         = ContactRecordInterface::GetNumberById(numberQuery->getID());
    auto response    = std::make_unique<db::query::NumberGetByIDResult>(ret);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::splitNumberIDs(const std::string &numberIDs) -> std::vector<std::uint32_t>
{
    std::stringstream source(numberIDs);
    return std::vector<std::uint32_t>(std::istream_iterator<std::uint32_t>(source),
                                      std::istream_iterator<std::uint32_t>());
}

auto ContactRecordInterface::joinNumberIDs(const std::vector<std::uint32_t> &numberIDs) -> std::string
{
    std::ostringstream outStream;
    std::ostream_iterator<std::uint32_t> outIterator(outStream, " ");
    std::copy(std::begin(numberIDs), std::end(numberIDs), outIterator);

    return outStream.str();
}

auto ContactRecordInterface::unbindNumber(std::uint32_t contactId, std::uint32_t numberId) -> bool
{
    auto contactRecord = contactDB->contacts.getById(contactId);
    if (!contactRecord.isValid()) {
        return false;
    }

    auto numberIDs = splitNumberIDs(contactRecord.numbersID);
    numberIDs.erase(std::remove(std::begin(numberIDs), std::end(numberIDs), numberId), std::end(numberIDs));
    contactRecord.numbersID = joinNumberIDs(numberIDs);

    return contactDB->contacts.update(contactRecord);
}

auto ContactRecordInterface::Update(const ContactRecord &rec) -> bool
{
    debug_db_data("%s", __FUNCTION__);
    bool ret;
    ContactsTableRow contact = contactDB->contacts.getByIdWithTemporary(rec.ID);
    if (!contact.isValid()) {
        return false;
    }

    std::vector<ContactNumberHolder> contactNumberHolders;
    auto numberMatcher                        = buildNumberMatcher(contactNumberHolders);
    std::vector<std::uint32_t> inputNumberIDs = splitNumberIDs(contact.numbersID);
    std::vector<std::uint32_t> outputNumberIDs;
    for (auto number : rec.numbers) {
        auto numberMatch =
            numberMatcher.bestMatch(utils::PhoneNumber(number.number), utils::PhoneNumber::Match::POSSIBLE);
        if (numberMatch == numberMatcher.END) {
            if (!contactDB->number.add(ContactsNumberTableRow{{.ID = DB_ID_NONE},
                                                              .contactID  = contact.ID,
                                                              .numberUser = number.number.getEntered().c_str(),
                                                              .numbere164 = number.number.getE164().c_str(),
                                                              .type       = number.numberType})) {
                error_db_data("Failed to add new number for contact");
                return false;
            }

            outputNumberIDs.push_back(contactDB->getLastInsertRowId());
        }
        else {
            if (auto oldId = numberMatch->getContactID(); oldId != rec.ID) {
                if (!unbindNumber(oldId, numberMatch->getNumberID())) {
                    error_db_data(
                        "Failed to unbind number %" PRIu32 " from contact %" PRIu32, numberMatch->getNumberID(), oldId);
                }

                auto numberRecord      = contactDB->number.getById(numberMatch->getNumberID());
                numberRecord.contactID = rec.ID;

                if (!contactDB->number.update(numberRecord)) {
                    error_db_data("Failed to reassing number %" PRIu32 " to contact %" PRIu32,
                                  numberMatch->getNumberID(),
                                  rec.ID);
                }
            }
            outputNumberIDs.push_back(numberMatch->getNumberID());
        }
    }

    // cleanup numbers that are missing in new ID list
    for (auto inNumberID : inputNumberIDs) {
        if (std::find(std::begin(outputNumberIDs), std::end(outputNumberIDs), inNumberID) ==
            std::end(outputNumberIDs)) {
            debug_db_data("Removing obsolete number from table: %" PRIu32, inNumberID);
            if (!contactDB->number.removeById(inNumberID)) {
                error_db_data("Failed to remove number");
                return false;
            }
        }
    }

    // check if speed dial is free to take, unbind if needed
    auto speedDialContacts = GetBySpeedDial(rec.speeddial);
    if (!speedDialContacts->empty()) {
        if (speedDialContacts->size() != 1) {
            error_db_data("Multiple contacts for same speed dial value %s", rec.speeddial.c_str());
        }
        auto oldContact      = contactDB->contacts.getById(speedDialContacts->at(0).ID);
        oldContact.speedDial = "";
        if (!contactDB->contacts.update(oldContact)) {
            error_db_data("Failed to remove speed dial from old contact");
        }
    }

    ret = contactDB->contacts.update(ContactsTableRow{{.ID = contact.ID},
                                                      .nameID          = contact.nameID,
                                                      .numbersID       = joinNumberIDs(outputNumberIDs),
                                                      .ringID          = contact.ringID,
                                                      .addressID       = contact.addressID,
                                                      .speedDial       = rec.speeddial,
                                                      .namePrimary     = rec.primaryName,
                                                      .nameAlternative = rec.alternativeName});

    if (!ret) {
        error_db_data("Failed to update contact.");
        return false;
    }

    ret = contactDB->name.update(ContactsNameTableRow{{.ID = contact.nameID},
                                                      .contactID       = contact.ID,
                                                      .namePrimary     = rec.primaryName,
                                                      .nameAlternative = rec.alternativeName});

    if (!ret) {
        error_db_data("Failed to update contact name");
        return false;
    }

    ret = contactDB->address.update(ContactsAddressTableRow{{.ID = contact.addressID},
                                                            .contactID = contact.ID,
                                                            .address   = rec.address,
                                                            .note      = rec.note,
                                                            .mail      = rec.mail});

    if (!ret) {
        error_db_data("Failed to update contact address");
        return false;
    }

    ret = contactDB->ringtones.update(ContactsRingtonesTableRow(contact.ID, rec.assetPath));

    if (!ret) {
        error_db_data("Failed to update contact ringtone");
        return false;
    }

    contactDB->groups.updateGroups(rec.ID, rec.groups);

    return true;
}

auto ContactRecordInterface::GetByID(uint32_t id) -> ContactRecord
{
    auto contact = contactDB->contacts.getById(id);
    return getByIdCommon(contact);
}

auto ContactRecordInterface::GetByIdWithTemporary(uint32_t id) -> ContactRecord
{
    debug_db_data("looking contact %" PRIu32 " with tmp", id);
    auto contact = contactDB->contacts.getByIdWithTemporary(id);
    return getByIdCommon(contact);
}

auto ContactRecordInterface::getByIdCommon(ContactsTableRow &contact) -> ContactRecord
{
    ContactRecord rec = ContactRecord();
    debug_db_data("%" PRIu32, contact.ID);
    if (!contact.isValid()) {
        return rec;
    }

    rec.ID        = contact.ID;
    rec.speeddial = contact.speedDial;
    rec.groups    = contactDB->groups.getGroupsForContact(contact.ID);

    auto numbers = getNumbers(contact.numbersID);
    if (numbers.size() > 0) {
        rec.numbers = numbers;
    }
    else {
        debug_db_data("Contact record does not contain any numbers.");
    }

    auto ring = contactDB->ringtones.getById(contact.ringID);
    if (ring.isValid()) {
        rec.assetPath = ring.assetPath;
    }
    else {
        debug_db_data("no ring record");
    }

    auto address = contactDB->address.getById(contact.addressID);
    if (address.isValid()) {
        rec.address = address.address;
        rec.note    = address.note;
        rec.mail    = address.mail;
    }
    else {
        debug_db_data("no addres record");
    }

    auto name = contactDB->name.getById(contact.nameID);
    if (name.isValid()) {
        rec.primaryName     = name.namePrimary;
        rec.alternativeName = name.nameAlternative;
    }
    else {
        debug_db_data("no name record");
    }

    return rec;
}

auto ContactRecordInterface::GetCount() -> uint32_t
{
    return contactDB->contacts.count();
}

auto ContactRecordInterface::GetCountFavourites() -> uint32_t
{
    return contactDB->contacts.countByFieldId("favourites", 1);
}

auto ContactRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->name.getLimitOffset(offset, limit);

    for (const auto &w : ret) {

        auto contact = contactDB->contacts.getById(w.ID);
        if (!contact.isValid()) {
            return records;
        }

        auto name = contactDB->name.getById(contact.nameID);
        if (!name.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            debug_db_data("Contact record does not contain any numbers.");
        }

        auto ring = contactDB->ringtones.getById(contact.ringID);
        if (!ring.isValid()) {
            return records;
        }

        auto address = contactDB->address.getById(contact.addressID);
        if (!address.isValid()) {
            return records;
        }

        records->push_back(ContactRecord{{.ID = contact.ID},
                                         .primaryName     = name.namePrimary,
                                         .alternativeName = name.nameAlternative,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .speeddial       = contact.speedDial,
                                         .groups          = contactDB->groups.getGroupsForContact(contact.ID)});
    }

    return records;
}

auto ContactRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                   uint32_t limit,
                                                   ContactRecordField field,
                                                   const char *str) -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    switch (field) {
    case ContactRecordField ::PrimaryName: {
        auto ret = contactDB->name.getLimitOffsetByField(offset, limit, ContactNameTableFields::NamePrimary, str);

        for (const auto &record : ret) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                debug_db_data("Contact record does not contain any numbers.");
            }

            auto ring = contactDB->ringtones.getById(contact.ringID);
            if (!ring.isValid()) {
                return records;
            }

            auto address = contactDB->address.getById(contact.addressID);
            if (!address.isValid()) {
                return records;
            }

            records->push_back(ContactRecord{{.ID = record.ID},
                                             .primaryName     = record.namePrimary,
                                             .alternativeName = record.nameAlternative,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .speeddial       = contact.speedDial,
                                             .groups          = contactDB->groups.getGroupsForContact(record.ID)});
        }
    } break;

    case ContactRecordField::NumberUser: {
        auto ret = contactDB->number.getLimitOffsetByField(offset, limit, ContactNumberTableFields ::NumberUser, str);

        for (const auto &record : ret) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                debug_db_data("Contact record does not contain any numbers.");
            }

            auto name = contactDB->name.getById(contact.nameID);
            if (!name.isValid()) {
                return records;
            }

            auto ring = contactDB->ringtones.getById(contact.ringID);
            if (!ring.isValid()) {
                return records;
            }

            auto address = contactDB->address.getById(contact.addressID);
            if (!address.isValid()) {
                return records;
            }

            records->push_back(ContactRecord{{.ID = record.ID},
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .speeddial       = contact.speedDial,
                                             .groups          = contactDB->groups.getGroupsForContact(record.ID)

            });
        }
    } break;

    case ContactRecordField::NumberE164: {
        auto ret = contactDB->number.getLimitOffsetByField(offset, limit, ContactNumberTableFields::NumberE164, str);

        for (const auto &record : ret) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                debug_db_data("Contact record does not contain any numbers.");
            }

            auto name = contactDB->name.getById(contact.nameID);
            if (!name.isValid()) {
                return records;
            }

            auto ring = contactDB->ringtones.getById(contact.ringID);
            if (!ring.isValid()) {
                return records;
            }

            auto address = contactDB->address.getById(contact.addressID);
            if (!address.isValid()) {
                return records;
            }

            records->push_back(ContactRecord{
                {.ID = record.ID},
                .primaryName     = name.namePrimary,
                .alternativeName = name.nameAlternative,
                .numbers         = nrs,
                .address         = address.address,
                .note            = address.note,
                .mail            = address.mail,
                .assetPath       = ring.assetPath,
                .speeddial       = contact.speedDial,
                .groups          = contactDB->groups.getGroupsForContact(record.ID),

            });
        }
    } break;
    case ContactRecordField::SpeedDial: {
        auto ret = contactDB->contacts.getLimitOffsetByField(0, 1, ContactTableFields::SpeedDial, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.getById(w.ID);
            if (!contact.isValid()) {
                return records;
            }

            auto name = contactDB->name.getById(contact.nameID);
            if (!name.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                debug_db_data("Contact record does not contain any numbers.");
            }

            auto ring = contactDB->ringtones.getById(contact.ringID);
            if (!ring.isValid()) {
                return records;
            }

            auto address = contactDB->address.getById(contact.addressID);
            if (!address.isValid()) {
                return records;
            }

            records->push_back(ContactRecord{{.ID = contact.ID},
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .speeddial       = contact.speedDial,
                                             .groups          = contactDB->groups.getGroupsForContact(contact.ID)});
        }
    } break;
    case ContactRecordField::Groups: {
        break;
    }
    }

    return records;
}

auto ContactRecordInterface::GetByName(UTF8 primaryName, UTF8 alternativeName)
    -> std::unique_ptr<std::vector<ContactRecord>>
{

    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->name.GetByName(primaryName.c_str(), alternativeName.c_str());

    for (const auto &record : ret) {

        auto contact = contactDB->contacts.getById(record.contactID);
        if (!contact.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            debug_db_data("Contact record does not contain any numbers.");
        }

        auto ring = contactDB->ringtones.getById(contact.ringID);
        if (!ring.isValid()) {
            return records;
        }

        auto address = contactDB->address.getById(contact.addressID);
        if (!address.isValid()) {
            return records;
        }

        records->push_back(ContactRecord{{.ID = record.ID},
                                         .primaryName     = record.namePrimary,
                                         .alternativeName = record.nameAlternative,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .speeddial       = contact.speedDial,
                                         .groups          = contactDB->groups.getGroupsForContact(record.ID)});
    }

    return records;
}

auto ContactRecordInterface::Search(const char *primaryName, const char *alternativeName, const char *number)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->contacts.Search(primaryName, alternativeName, number);

    for (const auto &record : ret) {
        auto contact = contactDB->contacts.getById(record.ID);
        if (!contact.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            debug_db_data("Contact record does not contain any numbers.");
        }

        auto ring = contactDB->ringtones.getById(contact.ringID);
        if (!ring.isValid()) {
            return records;
        }

        auto address = contactDB->address.getById(contact.addressID);
        if (!address.isValid()) {
            return records;
        }

        records->push_back(ContactRecord{{.ID = record.ID},
                                         .primaryName     = record.namePrimary,
                                         .alternativeName = record.nameAlternative,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .speeddial       = contact.speedDial,
                                         .groups          = contactDB->groups.getGroupsForContact(record.ID)});
    }

    return records;
}

auto ContactRecordInterface::getContactByNumber(const UTF8 &number) -> std::unique_ptr<std::vector<ContactRecord>>
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::NumberUser, number.c_str());
}

auto ContactRecordInterface::GetByNumber(const UTF8 &number, CreateTempContact createTempContact)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    return GetByNumber(utils::PhoneNumber(number.c_str(), utils::country::Id::UNKNOWN).getView(), createTempContact);
}

auto ContactRecordInterface::GetByNumber(const utils::PhoneNumber::View &numberView,
                                         CreateTempContact createTempContact)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    auto number = numberView.getEntered();
    auto ret    = getContactByNumber(number);
    if (ret->size() > 0) {
        return ret;
    }

    // Contact not found, create temporary one
    if (createTempContact == CreateTempContact::True) {
        debug_db_data("Cannot find contact for number %s, creating temporary one", number.c_str());
        ContactRecord tmpRecord = {
            {.ID = DB_ID_NONE},
            .numbers = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
        };
        if (!Add(tmpRecord)) {
            error_db_data("Cannot add contact record");
            return ret;
        }

        ret->push_back(GetByID(tmpRecord.ID));
    }

    return ret;
}

auto ContactRecordInterface::buildNumberMatcher(std::vector<ContactNumberHolder> &contactNumberHolders)
    -> utils::NumberHolderMatcher<std::vector, ContactNumberHolder>
{
    auto numbers = getAllNumbers();

    for (const auto &number : numbers) {
        try {
            contactNumberHolders.emplace_back(number);
        }
        catch (const utils::PhoneNumber::Error &e) {
            debug_db_data(
                "Skipping invalid phone number pair: (%s, %s)", number.numberUser.c_str(), number.numbere164.c_str());
            continue;
        }
    }

    return utils::NumberHolderMatcher<std::vector, ContactNumberHolder>(std::cbegin(contactNumberHolders),
                                                                        std::cend(contactNumberHolders));
}

auto ContactRecordInterface::MatchByNumber(const utils::PhoneNumber::View &numberView,
                                           CreateTempContact createTempContact,
                                           utils::PhoneNumber::Match matchLevel)
    -> std::optional<ContactRecordInterface::ContactNumberMatch>
{
    std::vector<ContactNumberHolder> contactNumberHolders;
    auto numberMatcher = buildNumberMatcher(contactNumberHolders);

    auto matchedNumber = numberMatcher.bestMatch(utils::PhoneNumber(numberView), matchLevel);

    if (matchedNumber == numberMatcher.END) {
        if (createTempContact != CreateTempContact::True) {
            return std::nullopt;
        }

        debug_db_data("Cannot find contact for number %s, creating temporary one", numberView.getEntered().c_str());
        ContactRecord newContact = {{.ID = DB_ID_NONE},
                                    .numbers = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
                                    .groups  = {contactDB->groups.getById(ContactsDB::temporaryGroupId())}};

        if (!Add(newContact)) {
            error_db_data("Cannot add contact record");
            return std::nullopt;
        }

        auto contactID       = newContact.ID;
        auto contactTableRow = contactDB->contacts.getByIdWithTemporary(contactID);
        auto numberIDs       = splitNumberIDs(contactTableRow.numbersID);
        assert(numberIDs.size() >= 1);
        auto numberID = numberIDs[0];
        return ContactRecordInterface::ContactNumberMatch(GetByIdWithTemporary(contactID), contactID, numberID);
    }

    auto contactID = matchedNumber->getContactID();
    auto numberID  = matchedNumber->getNumberID();
    return ContactRecordInterface::ContactNumberMatch(GetByIdWithTemporary(contactID), contactID, numberID);
}

auto ContactRecordInterface::GetBySpeedDial(UTF8 speedDial) -> std::unique_ptr<std::vector<ContactRecord>>
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::SpeedDial, speedDial.c_str());
}

auto ContactRecordInterface::getNumbers(const std::string &numbers_id) -> std::vector<ContactRecord::Number>
{
    std::vector<ContactRecord::Number> nrs;
    for (auto nr_str : utils::split(numbers_id, ' ')) {
        auto nr_val = 0L;
        try {
            nr_val = std::stol(nr_str);
        }
        catch (const std::exception &e) {
            error_db_data("Convertion error from %s, taking default value %ld", nr_str.c_str(), nr_val);
        }

        auto nr = contactDB->number.getById(nr_val);
        if (!nr.isValid()) {
            return nrs;
        }
        try {
            auto &&number = nr.numbere164.empty() ? utils::PhoneNumber(nr.numberUser, utils::country::Id::UNKNOWN)
                                                  : utils::PhoneNumber(nr.numberUser, nr.numbere164);
            nrs.emplace_back(number.getView(), nr.type);
        }
        catch (const utils::PhoneNumber::Error &e) {
            error_db_data(
                "Invalid contact's number pair: \"%s\" (\"%s\", \"%s\"). Using user number instead of a pair.",
                e.what(),
                nr.numberUser.c_str(),
                nr.numbere164.c_str());
            nrs.emplace_back(utils::PhoneNumber(nr.numberUser, utils::country::Id::UNKNOWN).getView(), nr.type);
        }
    }
    return nrs;
}

ContactRecord::Number::Number() = default;

ContactRecord::Number::Number(const std::string &entered, const std::string &e164, ContactNumberType n_type)
    : number(utils::PhoneNumber(entered, e164).getView()), numberType(n_type)
{}

ContactRecord::Number::Number(const utils::PhoneNumber::View &number, ContactNumberType n_type)
    : number(number), numberType(n_type)
{}

auto ContactRecordInterface::getAllNumbers() -> std::vector<ContactsNumberTableRow>
{
    static const std::size_t singleDumpSize = 64;
    std::vector<ContactsNumberTableRow> v;

    std::size_t offset       = 0;
    std::size_t numbersCount = contactDB->number.count();
    while (offset < numbersCount) {
        auto singleRead = contactDB->number.getLimitOffset(offset, singleDumpSize);
        v.insert(std::end(v), std::begin(singleRead), std::end(singleRead));
        offset += singleDumpSize;
    }

    return v;
}

ContactNumberHolder::ContactNumberHolder(ContactsNumberTableRow numberRow)
    : row(std::move(numberRow)), number(row.numbere164.empty() ? utils::PhoneNumber(row.numberUser)
                                                               : utils::PhoneNumber(row.numberUser, row.numbere164))
{}

auto ContactNumberHolder::getNumber() const -> const utils::PhoneNumber &
{
    return number;
}

auto ContactNumberHolder::getContactID() const -> std::uint32_t
{
    return row.contactID;
}

auto ContactNumberHolder::getNumberID() const -> std::uint32_t
{
    return row.ID;
}

void ContactRecord::addToFavourites(bool add)
{
    if (add) {
        groups.insert(ContactsDB::favouritesGroupId());
    }
    else {
        groups.erase(ContactsDB::favouritesGroupId());
    }
}

void ContactRecord::addToIce(bool add)
{
    if (add) {
        groups.insert(ContactsDB::iceGroupId());
    }
    else {
        groups.erase(ContactsDB::iceGroupId());
    }
}

void ContactRecord::addToBlocked(bool add)
{
    if (add) {
        groups.insert(ContactsDB::blockedGroupId());
        speeddial.clear();
    }
    else {
        groups.erase(ContactsDB::blockedGroupId());
    }
}

void ContactRecord::addToGroup(uint32_t groupId)
{
    groups.insert(groupId);
}

void ContactRecord::removeFromGroup(uint32_t groupId)
{
    groups.erase(groupId);
}

auto ContactRecord::isOnFavourites() const -> bool
{
    return isOnGroup(ContactsDB::favouritesGroupId());
}

auto ContactRecord::isOnIce() const -> bool
{
    return isOnGroup(ContactsDB::iceGroupId());
}

auto ContactRecord::isOnBlocked() const -> bool
{
    return isOnGroup(ContactsDB::blockedGroupId());
}

auto ContactRecord::isOnGroup(uint32_t groupId) const -> bool
{
    return groups.find(groupId) != groups.end();
}
auto ContactRecord::isTemporary() const -> bool
{
    return isOnGroup(ContactsDB::temporaryGroupId());
}

ContactRecordInterface::ContactNumberMatch::ContactNumberMatch(ContactRecord rec,
                                                               std::uint32_t contactId,
                                                               std::uint32_t numberId)
    : contact(std::move(rec)), contactId(contactId), numberId(numberId)
{}

auto ContactRecordInterface::GetNumberById(std::uint32_t numberId) -> utils::PhoneNumber::View
{
    auto row = contactDB->number.getById(numberId);
    return utils::PhoneNumber(row.numberUser, row.numbere164).getView();
}

auto ContactRecordInterface::GetNumbersIdsByContact(std::uint32_t contactId) -> std::vector<std::uint32_t>
{
    auto rows = contactDB->number.getByContactId(contactId);

    std::vector<std::uint32_t> numbersIds;
    numbersIds.reserve(rows.size());
    for (const auto &row : rows) {
        numbersIds.push_back(row.ID);
    }
    return numbersIds;
}
