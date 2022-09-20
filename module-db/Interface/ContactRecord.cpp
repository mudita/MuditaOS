// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactRecord.hpp"
#include "queries/phonebook/QueryContactAdd.hpp"
#include "queries/phonebook/QueryContactGetByID.hpp"
#include "queries/phonebook/QueryContactGetByNumberID.hpp"
#include "queries/phonebook/QueryContactUpdate.hpp"
#include "queries/phonebook/QueryContactRemove.hpp"
#include "queries/phonebook/QueryMergeContactsList.hpp"
#include "queries/phonebook/QueryCheckContactsListDuplicates.hpp"
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

namespace
{
    constexpr auto NumberMatcherPageSize = 100;
} // namespace

ContactRecordInterface::ContactRecordInterface(ContactsDB *db)
    : contactDB(db), favouritesGroupId(db->groups.favouritesId())
{}

auto ContactRecordInterface::Add(ContactRecord &rec) -> bool
{
    bool result = contactDB->contacts.add(ContactsTableRow{Record(DB_ID_NONE), .speedDial = rec.speeddial});
    if (!result) {
        return false;
    }
    uint32_t contactID = contactDB->getLastInsertRowId();
    LOG_DEBUG("New contact with ID %" PRIu32 " created", contactID);
    rec.ID = contactID;

    const auto numbersIDs = addNumbers(contactID, rec.numbers);
    if (!numbersIDs.has_value()) {
        return false;
    }
    if (!rec.isTemporary()) {
        const auto nameID = addOrUpdateName(contactID, DB_ID_NONE, rec);
        if (!nameID.has_value()) {
            return false;
        }
        const auto addressID = addOrUpdateAddress(contactID, DB_ID_NONE, rec);
        if (!addressID.has_value()) {
            return false;
        }
        const auto ringtoneID = addOrUpdateRingtone(contactID, DB_ID_NONE, rec);
        if (!ringtoneID.has_value()) {
            return false;
        }
        result = contactDB->contacts.update(ContactsTableRow{Record(contactID),
                                                             .nameID    = nameID.value(),
                                                             .numbersID = numbersIDs.value(),
                                                             .ringID    = ringtoneID.value(),
                                                             .addressID = addressID.value(),
                                                             .speedDial = rec.speeddial});
    }
    else {
        result = contactDB->contacts.update(ContactsTableRow{Record(contactID),
                                                             .nameID    = DB_ID_NONE,
                                                             .numbersID = numbersIDs.value(),
                                                             .ringID    = DB_ID_NONE,
                                                             .addressID = DB_ID_NONE,
                                                             .speedDial = rec.speeddial});
    }

    for (const auto &group : rec.groups) {
        contactDB->groups.addContactToGroup(contactID, group.ID);
    }
    return result;
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
        // Proceed if a contact is NOT a temporary contact.
        if (currentGroups.find(ContactsDB::temporaryGroupId()) == currentGroups.end()) {
            // Clear all the contact's data besides the phone numbers.
            contactDB->name.removeById(contact.nameID);
            contactDB->address.removeById(contact.addressID);
            contactDB->ringtones.removeById(contact.ringID);
            for (const auto &group : currentGroups) {
                contactDB->groups.removeContactFromGroup(id, group.ID);
            }
            contactDB->groups.addContactToGroup(id, ContactsDB::temporaryGroupId());

            contact.namePrimary.clear();
            contact.nameAlternative.clear();
            contact.speedDial.clear();
            contact.nameID    = DB_ID_NONE;
            contact.addressID = DB_ID_NONE;
            contact.ringID    = DB_ID_NONE;
            contactDB->contacts.update(contact);
        }
        // It's already a temporary contact, so it's already deleted.
    }
    return true;
}

auto ContactRecordInterface::Update(const ContactRecord &rec) -> bool
{
    ContactsTableRow contact = contactDB->contacts.getByIdWithTemporary(rec.ID);
    if (!contact.isValid()) {
        return false;
    }

    auto oldNumberIDs = splitNumberIDs(contact.numbersID);

    /* Changing number table record in place if new number is same as old number but with/without country code */
    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);
    for (const auto oldNumberID : oldNumberIDs) { // pick one of the old number for this contactID (from DB)
        auto numberRecord = contactDB->number.getById(oldNumberID);
        utils::PhoneNumber oldPhoneNumber(numberRecord.numberUser, numberRecord.numbere164);
        for (const auto &newNumberID : rec.numbers) { // pick one of the new number from &rec
            utils::PhoneNumber newPhoneNumber(newNumberID.number);
            // if DB have not such a new number already and if one of this have country code and other doesn't
            if (!numberMatcher.bestMatch(newPhoneNumber, utils::PhoneNumber::Match::EXACT).has_value() &&
                (newPhoneNumber.match(oldPhoneNumber) == utils::PhoneNumber::Match::POSSIBLE) &&
                ((!oldPhoneNumber.isValid() && newPhoneNumber.isValid()) ||
                 (oldPhoneNumber.isValid() && !newPhoneNumber.isValid()))) {
                // which means that only country code is to add or remove (change of country code is not supported here)
                // then change old number record in number table to the new number
                auto oldNumberRecordToUpdate       = contactDB->number.getById(oldNumberID);
                oldNumberRecordToUpdate.numberUser = newPhoneNumber.get();
                oldNumberRecordToUpdate.numbere164 = newPhoneNumber.toE164();
                if (!contactDB->number.update(oldNumberRecordToUpdate)) {
                    return false;
                }
            }
        }
    }

    auto newNumbersIDs = getNumbersIDs(contact.ID, rec, utils::PhoneNumber::Match::EXACT);
    for (auto oldNumberID : oldNumberIDs) {
        if (std::find(std::begin(newNumbersIDs), std::end(newNumbersIDs), oldNumberID) == std::end(newNumbersIDs)) {
            // If any oldNumberID is NOT one of the newNumbersID, which will be assigned to this contact, then
            // make temporary contact with this oldNumberID
            auto numberRecord = contactDB->number.getById(oldNumberID);
            if (!numberRecord.isValid()) {
                return false;
            }
            ContactRecord::Number number;
            try {
                number = ContactRecord::Number(numberRecord.numberUser, numberRecord.numbere164);
            }
            catch (const utils::PhoneNumber::Error &e) {
                LOG_ERROR("Exception in ContactRecord::Number while processing number %s! Reason: %s",
                          e.getNumber().c_str(),
                          e.what());
                return false;
            }
            // make temporary contact with old number
            const auto tmpContactRecord = addTemporaryContactForNumber(number);
            if (!tmpContactRecord.has_value()) {
                return false;
            }
            numberRecord.contactID = tmpContactRecord.value().ID;
            if (!contactDB->number.update(numberRecord)) {
                return false;
            }
        }
    }

    unbindSpeedDialKeyFromOtherContacts(rec.speeddial);

    const auto nameID = addOrUpdateName(contact.ID, contact.nameID, rec);
    if (!nameID.has_value()) {
        return false;
    }
    const auto addressID = addOrUpdateAddress(contact.ID, contact.addressID, rec);
    if (!addressID.has_value()) {
        return false;
    }
    const auto ringtoneID = addOrUpdateRingtone(contact.ID, contact.ringID, rec);
    if (!ringtoneID.has_value()) {
        return false;
    }

    ContactsTableRow row{Record(contact.ID),
                         .nameID          = nameID.value(),
                         .numbersID       = joinNumberIDs(newNumbersIDs),
                         .ringID          = ringtoneID.value(),
                         .addressID       = addressID.value(),
                         .speedDial       = rec.speeddial,
                         .namePrimary     = rec.primaryName,
                         .nameAlternative = rec.alternativeName};

    bool result       = false;
    bool recordExists = [&]() {
        auto record = contactDB->contacts.getById(contact.ID);
        return record.isValid();
    }();
    if (recordExists) {
        result = contactDB->contacts.update(row);
    }
    else {
        result = contactDB->contacts.add(row);
    }
    if (!result) {
        LOG_ERROR("Failed to update contact.");
        return false;
    }
    contactDB->groups.updateGroups(rec.ID, rec.groups);
    return true;
}

auto ContactRecordInterface::getNumbersIDs(std::uint32_t contactID,
                                           const ContactRecord &contact,
                                           utils::PhoneNumber::Match matchLevel) -> std::vector<std::uint32_t>
{
    std::vector<std::uint32_t> result;

    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);
    for (const auto &number : contact.numbers) {
        utils::PhoneNumber phoneNumber;
        try {
            phoneNumber = utils::PhoneNumber(number.number);
        }
        catch (const utils::PhoneNumber::Error &e) {
            LOG_ERROR("Exception in utils::PhoneNumber while processing number %s! Reason: %s",
                      e.getNumber().c_str(),
                      e.what());
            return {};
        }

        auto numberMatch = numberMatcher.bestMatch(phoneNumber, matchLevel);
        if (!numberMatch.has_value()) {
            // number does not exist in the DB yet. Let's add it.
            if (!contactDB->number.add(ContactsNumberTableRow{Record(DB_ID_NONE),
                                                              .contactID  = contactID,
                                                              .numberUser = number.number.getEntered(),
                                                              .numbere164 = number.number.getE164(),
                                                              .type       = number.numberType})) {
                error_db_data("Failed to add new number for contact");
                return {};
            }
            result.push_back(contactDB->getLastInsertRowId());
        }
        else {
            // number already exists in the DB.
            if (const auto oldContactId = numberMatch->getContactID(); oldContactId != contact.ID) {
                // It's assigned to another contact. Unbind it from the old contact and bind it the new one.
                if (!unbindNumber(oldContactId, numberMatch->getNumberID())) {
                    LOG_ERROR("Failed to unbind number %" PRIu32 " from contact %" PRIu32,
                              numberMatch->getNumberID(),
                              oldContactId);
                    continue;
                }

                auto numberRecord      = contactDB->number.getById(numberMatch->getNumberID());
                numberRecord.contactID = contact.ID;
                if (!contactDB->number.update(numberRecord)) {
                    LOG_ERROR("Failed to re-assign number %" PRIu32 " to contact %" PRIu32,
                              numberMatch->getNumberID(),
                              contact.ID);
                    continue;
                }
            }
            result.push_back(numberMatch->getNumberID());
        }
    }
    return result;
}

auto ContactRecordInterface::addNumbers(std::uint32_t contactID, const std::vector<ContactRecord::Number> &numbers)
    -> std::optional<std::string>
{
    std::string numbersIDs;
    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);
    for (const auto &number : numbers) {
        utils::PhoneNumber phoneNumber;
        try {
            phoneNumber = utils::PhoneNumber(number.number);
        }
        catch (const utils::PhoneNumber::Error &e) {
            LOG_ERROR("Exception in utils::PhoneNumber while processing number %s! Reason: %s",
                      e.getNumber().c_str(),
                      e.what());
            return std::nullopt;
        }
        auto numberMatch = numberMatcher.bestMatch(phoneNumber, utils::PhoneNumber::Match::POSSIBLE);
        if (!numberMatch.has_value()) {
            auto result = contactDB->number.add(ContactsNumberTableRow{Record(DB_ID_NONE),
                                                                       .contactID  = contactID,
                                                                       .numberUser = number.number.getEntered(),
                                                                       .numbere164 = number.number.getE164(),
                                                                       .type       = number.numberType});
            if (!result) {
                return std::nullopt;
            }
            numbersIDs += std::to_string(contactDB->getLastInsertRowId()) + " ";
        }
        else {
            auto numberRecord = contactDB->number.getById(numberMatch->getNumberID());
            if (!unbindNumber(numberRecord.contactID, numberRecord.ID)) {
                return std::nullopt;
            }
            numberRecord.contactID = contactID;
            if (!contactDB->number.update(numberRecord)) {
                return std::nullopt;
            }
            numbersIDs += std::to_string(numberRecord.ID) + " ";
        }
    }
    if (!numbersIDs.empty()) {
        numbersIDs.pop_back(); // remove trailing space
    }
    return numbersIDs;
}

auto ContactRecordInterface::addOrUpdateName(std::uint32_t contactID,
                                             std::uint32_t nameID,
                                             const ContactRecord &contact) -> std::optional<std::uint32_t>
{
    if (nameID == DB_ID_NONE) {
        bool result = contactDB->name.add(ContactsNameTableRow{Record(DB_ID_NONE),
                                                               .contactID       = contactID,
                                                               .namePrimary     = contact.primaryName,
                                                               .nameAlternative = contact.alternativeName});
        if (!result) {
            error_db_data("Failed to add contact name");
            return std::nullopt;
        }
        nameID = contactDB->getLastInsertRowId();
    }
    else {
        bool result = contactDB->name.update(ContactsNameTableRow{Record(nameID),
                                                                  .contactID       = contactID,
                                                                  .namePrimary     = contact.primaryName,
                                                                  .nameAlternative = contact.alternativeName});
        if (!result) {
            error_db_data("Failed to update contact name");
            return std::nullopt;
        }
    }
    return nameID;
}

auto ContactRecordInterface::addOrUpdateAddress(std::uint32_t contactID,
                                                std::uint32_t addressID,
                                                const ContactRecord &contact) -> std::optional<std::uint32_t>
{
    if (addressID == DB_ID_NONE) {
        bool result = contactDB->address.add(ContactsAddressTableRow{Record(DB_ID_NONE),
                                                                     .contactID = contactID,
                                                                     .address   = contact.address,
                                                                     .note      = contact.note,
                                                                     .mail      = contact.mail});
        if (!result) {
            error_db_data("Failed to add contact address");
            return std::nullopt;
        }
        addressID = contactDB->getLastInsertRowId();
    }
    else {
        bool result = contactDB->address.update(ContactsAddressTableRow{Record(addressID),
                                                                        .contactID = contactID,
                                                                        .address   = contact.address,
                                                                        .note      = contact.note,
                                                                        .mail      = contact.mail});
        if (!result) {
            error_db_data("Failed to update contact address");
            return std::nullopt;
        }
    }
    return addressID;
}

auto ContactRecordInterface::addOrUpdateRingtone(std::uint32_t contactID,
                                                 std::uint32_t ringtoneID,
                                                 const ContactRecord &contact) -> std::optional<std::uint32_t>
{
    if (ringtoneID == DB_ID_NONE) {
        bool result = contactDB->ringtones.add(ContactsRingtonesTableRow{contact.ID, contact.assetPath});
        if (!result) {
            error_db_data("Failed to add contact ringtone");
            return std::nullopt;
        }
        ringtoneID = contactDB->getLastInsertRowId();
    }
    else {
        bool result = contactDB->ringtones.update(ContactsRingtonesTableRow{ringtoneID, contact.ID, contact.assetPath});
        if (!result) {
            error_db_data("Failed to update contact ringtone");
            return std::nullopt;
        }
    }
    return ringtoneID;
}

auto ContactRecordInterface::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
{
    if (typeid(*query) == typeid(db::query::ContactGet)) {
        return getQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetWithTotalCount)) {
        return getQueryWithTotalCount(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetLetterMap)) {
        return getLetterMapQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetByID)) {
        return getByIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetByNumberID)) {
        return getByNumberIDQuery(query);
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
    else if (typeid(*query) == typeid(db::query::MergeContactsList)) {
        return mergeContactsListQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::CheckContactsListDuplicates)) {
        return checkContactsListDuplicatesQuery(query);
    }
    error_db_data("Unexpected query type.");
    return nullptr;
}

auto ContactRecordInterface::getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<ContactRecord>
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
    return result;
}

auto ContactRecordInterface::getQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::ContactGetResult>(getQueryRecords(query));
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getQueryWithTotalCount(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    if (auto queryContacts = dynamic_cast<db::query::ContactGet *>(query.get())) {
        auto querySize = std::make_shared<db::query::ContactGetSize>(queryContacts->getFilterData(),
                                                                     queryContacts->getGroupFilterData(),
                                                                     queryContacts->getContactDisplayMode());
        auto response  = std::make_unique<db::query::ContactGetResultWithTotalCount>(getQueryRecords(query),
                                                                                    getContactsSize(querySize));
        response->setRequestQuery(query);
        return response;
    }
    return nullptr;
}

auto ContactRecordInterface::getLetterMapQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    ContactsMapData result = contactDB->contacts.GetPosOfFirstLetters();
    auto response          = std::make_unique<db::query::ContactGetLetterMapResult>(result);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getByIDQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
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

auto ContactRecordInterface::getByNumberIDQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto readQuery = static_cast<db::query::ContactGetByNumberID *>(query.get());

    ContactRecord contactRecord;
    auto numberRecord = contactDB->number.getById(readQuery->numberID);
    if (numberRecord.isValid()) {
        auto row      = contactDB->contacts.getById(numberRecord.contactID);
        contactRecord = getByIdCommon(row);
    }

    auto response = std::make_unique<db::query::ContactGetByNumberIDResult>(contactRecord);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getContactsSize(const std::shared_ptr<db::Query> &query) -> std::size_t
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
    return count;
}

auto ContactRecordInterface::getSizeQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto count = getContactsSize(query);
    debug_db_data("Contact count query result: %lu", static_cast<unsigned long>(count));

    auto response = std::make_unique<db::query::RecordsSizeQueryResult>(count);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::getForListQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    return nullptr;
}

auto ContactRecordInterface::addQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>
{
    auto addQuery = dynamic_cast<db::query::ContactAdd *>(query.get());
    if (addQuery == nullptr) {
        LOG_ERROR("Dynamic casting db::query::ContactAdd has failed!");
        return std::unique_ptr<db::QueryResult>();
    }
    auto result = false;

    auto duplicateCheckResult = verifyDuplicate(addQuery->rec);
    if (!duplicateCheckResult) {
        result = ContactRecordInterface::Add(addQuery->rec);
    }

    auto response = std::make_unique<db::query::ContactAddResult>(result, addQuery->rec.ID, duplicateCheckResult);
    response->setRequestQuery(query);
    response->setRecordID(addQuery->rec.ID);
    return response;
}

auto ContactRecordInterface::updateQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto updateQuery = static_cast<const db::query::ContactUpdate *>(query.get());
    auto result      = ContactRecordInterface::Update(updateQuery->rec);
    auto response    = std::make_unique<db::query::ContactUpdateResult>(result);
    response->setRequestQuery(query);
    response->setRecordID(updateQuery->rec.ID);
    return response;
}

auto ContactRecordInterface::removeQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto removeQuery = static_cast<db::query::ContactRemove *>(query.get());
    auto result      = ContactRecordInterface::RemoveByID(removeQuery->getID());
    auto response    = std::make_unique<db::query::ContactRemoveResult>(result);
    response->setRequestQuery(query);
    response->setRecordID(removeQuery->getID());
    return response;
}

auto ContactRecordInterface::numberGetByIdQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto numberQuery = static_cast<db::query::NumberGetByID *>(query.get());
    auto result      = ContactRecordInterface::GetNumberById(numberQuery->getID());
    auto response    = std::make_unique<db::query::NumberGetByIDResult>(result);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::mergeContactsListQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto mergeQuery = static_cast<db::query::MergeContactsList *>(query.get());
    auto result     = ContactRecordInterface::MergeContactsList(mergeQuery->getContactsList());
    auto response   = std::make_unique<db::query::MergeContactsListResult>(result);
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::checkContactsListDuplicatesQuery(const std::shared_ptr<db::Query> &query)
    -> const std::unique_ptr<db::QueryResult>
{
    auto mergeQuery = static_cast<db::query::CheckContactsListDuplicates *>(query.get());
    auto response   = std::make_unique<db::query::CheckContactsListDuplicatesResult>(
        ContactRecordInterface::CheckContactsListDuplicates(mergeQuery->getContactsList()));
    response->setRequestQuery(query);
    return response;
}

auto ContactRecordInterface::splitNumberIDs(const std::string &numberIDs) -> const std::vector<std::uint32_t>
{
    std::stringstream source(numberIDs);
    return std::vector<std::uint32_t>(std::istream_iterator<std::uint32_t>(source),
                                      std::istream_iterator<std::uint32_t>());
}

auto ContactRecordInterface::joinNumberIDs(const std::vector<std::uint32_t> &numberIDs) -> std::string
{
    if (numberIDs.empty()) {
        return {};
    }

    std::ostringstream outStream;
    std::ostream_iterator<std::uint32_t> outIterator(outStream, " ");
    std::copy(std::begin(numberIDs), std::end(numberIDs), outIterator);
    return outStream.str();
}

auto ContactRecordInterface::unbindNumber(std::uint32_t contactId, std::uint32_t numberId) -> bool
{
    auto contactRecord = contactDB->contacts.getByIdWithTemporary(contactId);
    if (!contactRecord.isValid()) {
        return false;
    }
    auto numberRecord = contactDB->number.getById(numberId);
    if (!numberRecord.isValid()) {
        return false;
    }

    // unbind contact from number
    numberRecord.contactID = DB_ID_NONE;
    if (!contactDB->number.update(numberRecord)) {
        return false;
    }

    // unbind number from contact
    auto numberIDs = splitNumberIDs(contactRecord.numbersID);
    numberIDs.erase(std::remove(std::begin(numberIDs), std::end(numberIDs), numberId), std::end(numberIDs));
    contactRecord.numbersID = joinNumberIDs(numberIDs);
    return contactDB->contacts.update(contactRecord);
}

void ContactRecordInterface::unbindSpeedDialKeyFromOtherContacts(const UTF8 &key)
{
    auto speedDialContacts = GetBySpeedDial(key);
    if (speedDialContacts->empty()) {
        return;
    }

    for (const auto &contact : *(speedDialContacts)) {
        auto oldContact = contactDB->contacts.getById(contact.ID);
        oldContact.speedDial.clear();
        if (!contactDB->contacts.update(oldContact)) {
            error_db_data("Failed to remove speed dial from old contact");
        }
    }
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
    if (!numbers.empty()) {
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

    auto result = contactDB->name.getLimitOffset(offset, limit);

    for (const auto &w : result) {

        auto contact = contactDB->contacts.getById(w.ID);
        if (!contact.isValid()) {
            return records;
        }

        auto name = contactDB->name.getById(contact.nameID);
        if (!name.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.empty()) {
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

        records->push_back(ContactRecord{Record(contact.ID),
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
    case ContactRecordField::PrimaryName: {
        auto result = contactDB->name.getLimitOffsetByField(offset, limit, ContactNameTableFields::NamePrimary, str);

        for (const auto &record : result) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.empty()) {
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

            records->push_back(ContactRecord{Record(record.ID),
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
        auto ret = contactDB->number.getLimitOffsetByField(offset, limit, ContactNumberTableFields::NumberUser, str);

        for (const auto &record : ret) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.empty()) {
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

            records->push_back(ContactRecord{Record(record.ID),
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
        auto result = contactDB->number.getLimitOffsetByField(offset, limit, ContactNumberTableFields::NumberE164, str);

        for (const auto &record : result) {

            auto contact = contactDB->contacts.getById(record.contactID);
            if (!contact.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.empty()) {
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
                Record(record.ID),
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
        auto result = contactDB->contacts.getLimitOffsetByField(0, 1, ContactTableFields::SpeedDial, str);

        for (const auto &w : result) {

            auto contact = contactDB->contacts.getById(w.ID);
            if (!contact.isValid()) {
                return records;
            }

            auto name = contactDB->name.getById(contact.nameID);
            if (!name.isValid()) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.empty()) {
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

            records->push_back(ContactRecord{Record(contact.ID),
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

auto ContactRecordInterface::GetByName(const UTF8 &primaryName, const UTF8 &alternativeName)
    -> std::unique_ptr<std::vector<ContactRecord>>
{

    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto result = contactDB->name.GetByName(primaryName.c_str(), alternativeName.c_str());

    for (const auto &record : result) {

        auto contact = contactDB->contacts.getById(record.contactID);
        if (!contact.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.empty()) {
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

        records->push_back(ContactRecord{Record(record.ID),
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

    auto result = contactDB->contacts.Search(primaryName, alternativeName, number);

    for (const auto &record : result) {
        auto contact = contactDB->contacts.getById(record.ID);
        if (!contact.isValid()) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.empty()) {
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

        records->push_back(ContactRecord{Record(record.ID),
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

auto ContactRecordInterface::getContactByNumber(const UTF8 &number) -> const std::unique_ptr<std::vector<ContactRecord>>
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::NumberUser, number.c_str());
}

auto ContactRecordInterface::GetByNumber(const UTF8 &number, CreateTempContact createTempContact)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    try {
        return GetByNumber(utils::PhoneNumber(number.c_str(), utils::country::Id::UNKNOWN).getView(),
                           createTempContact);
    }
    catch (const utils::PhoneNumber::Error &e) {
        LOG_ERROR(
            "Exception in utils::PhoneNumber while processing number %s! Reason: %s", e.getNumber().c_str(), e.what());
        return {};
    }
}

auto ContactRecordInterface::GetByNumber(const utils::PhoneNumber::View &numberView,
                                         CreateTempContact createTempContact)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    const auto &number = numberView.getEntered();
    auto result        = getContactByNumber(number);
    if (!result->empty()) {
        return result;
    }

    // Contact not found, create temporary one
    if (createTempContact == CreateTempContact::True) {
        debug_db_data("Cannot find contact for number %s, creating temporary one", number.c_str());
        const auto tmpRecord = addTemporaryContactForNumber(ContactRecord::Number(numberView));
        if (!tmpRecord.has_value()) {
            return result;
        }
        result->push_back(GetByID(tmpRecord.value().ID));
    }
    return result;
}

auto ContactRecordInterface::GetByNumberID(std::uint32_t numberId) -> std::optional<ContactRecord>
{
    auto numberRecord = contactDB->number.getById(numberId);
    if (!numberRecord.isValid()) {
        return std::nullopt;
    }

    auto rawContactRecord = contactDB->contacts.getByIdWithTemporary(numberRecord.contactID);
    auto contactRecord    = getByIdCommon(rawContactRecord);
    if (!contactRecord.isValid()) {
        return std::nullopt;
    }
    return contactRecord;
}

auto ContactRecordInterface::addTemporaryContactForNumber(const ContactRecord::Number &number)
    -> std::optional<ContactRecord>
{
    ContactRecord tmp;
    tmp.numbers = std::vector<ContactRecord::Number>{number};
    tmp.addToGroup(contactDB->groups.temporaryId());
    if (!Add(tmp)) {
        error_db_data("Cannot add contact record");
        return std::nullopt;
    }
    return tmp;
}

auto ContactRecordInterface::buildNumberMatcher(unsigned int maxPageSize)
    -> utils::NumberHolderMatcher<std::vector, ContactNumberHolder>
{
    return utils::NumberHolderMatcher<std::vector, ContactNumberHolder>(
        [this](const utils::PhoneNumber &number, auto offset, auto limit) {
            auto numbers = !number.get().empty() ? contactDB->number.getLimitOffset(number.get(), offset, limit)
                                                 : contactDB->number.getLimitOffset(offset, limit);

            std::vector<ContactNumberHolder> contactNumberHolders;
            contactNumberHolders.reserve(numbers.size());
            std::move(numbers.begin(), numbers.end(), std::back_inserter(contactNumberHolders));
            return contactNumberHolders;
        },
        maxPageSize);
}

auto ContactRecordInterface::MatchByNumber(const utils::PhoneNumber::View &numberView,
                                           CreateTempContact createTempContact,
                                           utils::PhoneNumber::Match matchLevel,
                                           const std::uint32_t contactIDToIgnore)
    -> std::optional<ContactRecordInterface::ContactNumberMatch>
{
    utils::PhoneNumber phoneNumber;
    try {
        phoneNumber = utils::PhoneNumber(numberView);
    }
    catch (const utils::PhoneNumber::Error &e) {
        LOG_ERROR(
            "Exception in utils::PhoneNumber while processing number %s! Reason: %s", e.getNumber().c_str(), e.what());
        return std::nullopt;
    }

    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);
    auto matchedNumber = numberMatcher.bestMatch(phoneNumber, matchLevel, contactIDToIgnore);
    if (!matchedNumber.has_value()) {
        if (createTempContact != CreateTempContact::True) {
            return std::nullopt;
        }

        debug_db_data("Cannot find contact for number %s, creating temporary one", numberView.getEntered().c_str());
        ContactRecord newContact = {Record(DB_ID_NONE),
                                    .numbers = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
                                    .groups  = {contactDB->groups.getById(ContactsDB::temporaryGroupId())}};

        if (!Add(newContact)) {
            error_db_data("Cannot add contact record");
            return std::nullopt;
        }

        auto contactID       = newContact.ID;
        auto contactTableRow = contactDB->contacts.getByIdWithTemporary(contactID);
        auto numberIDs       = splitNumberIDs(contactTableRow.numbersID);
        assert(!numberIDs.empty());
        auto numberID = numberIDs[0];
        return ContactRecordInterface::ContactNumberMatch(GetByIdWithTemporary(contactID), contactID, numberID);
    }

    auto contactID = matchedNumber->getContactID();
    auto numberID  = matchedNumber->getNumberID();
    return ContactRecordInterface::ContactNumberMatch(GetByIdWithTemporary(contactID), contactID, numberID);
}

auto ContactRecordInterface::GetBySpeedDial(const UTF8 &speedDial) -> std::unique_ptr<std::vector<ContactRecord>>
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::SpeedDial, speedDial.c_str());
}

auto ContactRecordInterface::getNumbers(const std::string &numbers_id) -> std::vector<ContactRecord::Number>
{
    std::vector<ContactRecord::Number> nrs;
    for (const auto &nr_str : utils::split(numbers_id, ' ')) {
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

auto ContactRecordInterface::getAllNumbers() -> const std::vector<ContactsNumberTableRow>
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

ContactNumberHolder::ContactNumberHolder(ContactsNumberTableRow &&numberRow) : numberRow(std::move(numberRow))
{}

auto ContactNumberHolder::getNumber() const noexcept -> utils::PhoneNumber
{
    try {
        return utils::PhoneNumber{numberRow.numbere164.empty()
                                      ? utils::PhoneNumber(numberRow.numberUser)
                                      : utils::PhoneNumber(numberRow.numberUser, numberRow.numbere164)};
    }
    catch (const utils::PhoneNumber::Error &) {
        debug_db_data(
            "Skipping invalid phone number pair: (%s, %s)", numberRow.numberUser.c_str(), numberRow.numbere164.c_str());
        return utils::PhoneNumber{};
    }
}

auto ContactNumberHolder::getContactID() const noexcept -> std::uint32_t
{
    return numberRow.contactID;
}

auto ContactNumberHolder::getNumberID() const noexcept -> std::uint32_t
{
    return numberRow.ID;
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
    const auto row = contactDB->number.getById(numberId);
    try {
        return utils::PhoneNumber(row.numberUser, row.numbere164).getView();
    }
    catch (const utils::PhoneNumber::Error &e) {
        LOG_ERROR(
            "Exception in utils::PhoneNumber while processing number %s! Reason: %s", e.getNumber().c_str(), e.what());
        return utils::PhoneNumber().getView();
    }
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

auto ContactRecordInterface::MergeContactsList(std::vector<ContactRecord> &contacts) -> bool
{
    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);

    for (auto &contact : contacts) {
        // Important: Comparing only single number contacts
        if (contact.numbers.size() > 1) {
            LOG_WARN("Contact with multiple numbers detected - ignoring all numbers except first");
        }
        auto matchedNumber = numberMatcher.bestMatch(contact.numbers[0].number, utils::PhoneNumber::Match::POSSIBLE);

        if (!matchedNumber.has_value()) {
            if (!Add(contact)) {
                LOG_ERROR("Contacts list merge fail when adding the contact.");
                return false;
            }
        }
        else {
            // Complete override of the contact data
            contact.ID = matchedNumber->getContactID();
            Update(contact);
            // Rebuild number matcher
            numberMatcher = buildNumberMatcher(NumberMatcherPageSize);
        }
    }
    return true;
}

auto ContactRecordInterface::CheckContactsListDuplicates(std::vector<ContactRecord> &contacts)
    -> std::pair<std::vector<ContactRecord>, std::vector<ContactRecord>>
{
    std::vector<ContactRecord> unique;
    std::vector<ContactRecord> duplicates;
    auto numberMatcher = buildNumberMatcher(NumberMatcherPageSize);

    for (auto &contact : contacts) {
        // Important: Comparing only single number contacts
        if (contact.numbers.size() > 1) {
            LOG_WARN("Contact with multiple numbers detected - ignoring all numbers except first");
        }
        auto matchedNumber = numberMatcher.bestMatch(contact.numbers[0].number, utils::PhoneNumber::Match::POSSIBLE);
        if (matchedNumber.has_value()) {
            duplicates.push_back(contact);
        }
        else {
            unique.push_back(contact);
        }
    }
    return {unique, duplicates};
}

auto ContactRecordInterface::verifyDuplicate(ContactRecord &record) -> bool
{
    auto isDuplicate = false;
    for (const auto &number : record.numbers) {
        auto matchResult = MatchByNumber(number.number);
        if (!matchResult.has_value() || matchResult.value().contact.isTemporary()) {
            continue;
        }
        isDuplicate = true;
        record.ID   = matchResult.value().contactId;
        break;
    }
    return isDuplicate;
}

auto ContactRecordInterface::verifyTemporary(ContactRecord &record) -> bool
{
    auto isTemporary = false;
    for (const auto &number : record.numbers) {
        auto matchResult = MatchByNumber(number.number);
        if (!matchResult.has_value() || !matchResult.value().contact.isTemporary()) {
            continue;
        }
        isTemporary = true;
        record.ID   = matchResult.value().contactId;
        break;
    }

    return isTemporary;
}
