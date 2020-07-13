#include "ContactRecord.hpp"
#include <Utils.hpp>

#include <queries/phonebook/QueryContactGet.hpp>

#include <PhoneNumber.hpp>
#include <NumberHolderMatcher.hpp>

#include <algorithm>
#include <optional>
#include <vector>

ContactRecordInterface::ContactRecordInterface(ContactsDB *db) : contactDB(db)
{}

ContactRecordInterface::~ContactRecordInterface()
{}

bool ContactRecordInterface::Add(const ContactRecord &rec)
{

    bool ret = contactDB->contacts.Add(ContactsTableRow{.type           = rec.contactType,
                                                        .isOnWhitelist  = rec.isOnBlacklist,
                                                        .isOnBlacklist  = rec.isOnBlacklist,
                                                        .isOnFavourites = rec.isOnFavourites,
                                                        .speedDial      = rec.speeddial,
                                                        .namePrimary    = rec.primaryName});

    if (!ret) {
        return ret;
    }

    uint32_t contactID = contactDB->GetLastInsertRowID();
    LOG_DEBUG("New contact with ID %" PRIu32 " created", contactID);

    ret = contactDB->name.Add(ContactsNameTableRow{.contactID       = contactID,
                                                   .namePrimary     = rec.primaryName,
                                                   .nameAlternative = rec.alternativeName,
                                                   .favourite       = rec.isOnFavourites});

    if (!ret) {
        return ret;
    }

    auto contactNameID = contactDB->GetLastInsertRowID();

    for (auto a : rec.numbers) {
        ret = contactDB->number.Add(ContactsNumberTableRow{.contactID  = contactID,
                                                           .numberUser = a.number.getEntered().c_str(),
                                                           .numbere164 = a.number.getE164().c_str(),
                                                           .type       = a.numberType});
    }

    if (!ret) {
        return ret;
    }

    auto contactNumberID = contactDB->GetLastInsertRowID();

    ret = contactDB->ringtones.Add(
        ContactsRingtonesTableRow{.ID = DB_ID_NONE, .contactID = contactID, .assetPath = rec.assetPath});

    if (!ret) {
        return ret;
    }

    auto contactRingID = contactDB->GetLastInsertRowID();

    ret = contactDB->address.Add(
        ContactsAddressTableRow{.contactID = contactID, .address = rec.address, .note = rec.note, .mail = rec.mail});

    if (!ret) {
        return ret;
    }

    auto contactAddressID = contactDB->GetLastInsertRowID();

    ret = contactDB->contacts.Update(ContactsTableRow{.ID        = contactID,
                                                      .nameID    = contactNameID,
                                                      .numbersID = std::to_string(contactNumberID),
                                                      .ringID    = contactRingID,
                                                      .addressID = contactAddressID,

                                                      .type           = rec.contactType,
                                                      .isOnWhitelist  = rec.isOnBlacklist,
                                                      .isOnBlacklist  = rec.isOnBlacklist,
                                                      .isOnFavourites = rec.isOnFavourites,
                                                      .speedDial      = rec.speeddial});

    return ret;
}

bool ContactRecordInterface::BlockByID(uint32_t id, const bool shouldBeBlocked)
{
    return contactDB->contacts.BlockByID(id, shouldBeBlocked);
}

bool ContactRecordInterface::RemoveByID(uint32_t id)
{

    auto contact = contactDB->contacts.GetByID(id);
    if (contact.ID == 0) {
        return false;
    }

    if (contactDB->name.RemoveByID(contact.nameID) == false) {
        return false;
    }

    if (contactDB->address.RemoveByID(contact.addressID) == false) {
        return false;
    }

    if (contactDB->number.RemoveByID(std::stoul(contact.numbersID)) == false) {
        return false;
    }

    if (contactDB->ringtones.RemoveByID(contact.ringID) == false) {
        return false;
    }

    if (contactDB->contacts.RemoveByID(id) == false) {
        return false;
    }

    return true;
}

std::unique_ptr<db::QueryResult> ContactRecordInterface::runQuery(const db::Query *query)
{
    if (query->type != db::Query::Type::Read) {
        LOG_WARN("Received unhandled query type: %lu", static_cast<unsigned long int>(query->type));
        return nullptr;
    }

    auto textFilter = dynamic_cast<const db::query::TextFilter *>(query);
    assert(query != nullptr);
    bool searchByNumber = false;
    if (textFilter->isFilterPresent() && utils::is_number(textFilter->getFilterData())) {
        searchByNumber = true;
        LOG_INFO("Filtering by number: %s", textFilter->getFilterData().c_str());
    }

    if (typeid(*query) == typeid(db::query::ContactGet)) {
        auto readQuery = static_cast<const db::query::ContactGet *>(query);
        LOG_DEBUG("Contact read query, filter: \"%s\", offset=%lu, limit=%lu",
                  readQuery->getFilterData().c_str(),
                  static_cast<unsigned long>(readQuery->getOffset()),
                  static_cast<unsigned long>(readQuery->getLimit()));
        auto [limit, offset] = readQuery->getLimitOffset();
        auto ids = searchByNumber ? contactDB->contacts.GetIDsByTextNumber(readQuery->getFilterData(), limit, offset)
                                  : contactDB->name.GetIDsByName(readQuery->getFilterData(), limit, offset);

        std::vector<ContactRecord> result(ids.size());
        std::transform(std::begin(ids), std::end(ids), std::begin(result), [this](uint32_t id) { return GetByID(id); });

        return std::make_unique<db::query::ContactGetResult>(result);
    }
    else if (typeid(*query) == typeid(db::query::ContactGetSize)) {
        auto countQuery = static_cast<const db::query::ContactGetSize *>(query);
        LOG_DEBUG("Contact count query, filter: \"%s\"", countQuery->getFilterData().c_str());

        std::size_t count = 0;
        if (!countQuery->isFilterPresent()) {
            count = contactDB->contacts.GetCount();
        }
        else if (searchByNumber) {
            count = contactDB->contacts.GetIDsByTextNumber(countQuery->getFilterData()).size();
        }
        else {
            count = contactDB->name.GetCountByName(countQuery->getFilterData());
        }

        LOG_DEBUG("Contact count query result: %lu", static_cast<unsigned long>(count));

        return std::make_unique<db::query::RecordsSizeQueryResult>(count);
    }
    else {
        LOG_ERROR("Unexpected query type.");
        return nullptr;
    }
}

bool ContactRecordInterface::Update(const ContactRecord &rec)
{
    ContactsTableRow contact = contactDB->contacts.GetByID(rec.ID);
    if (contact.ID == DB_ID_NONE)
        return false;

    bool ret = contactDB->contacts.Update(ContactsTableRow{.ID              = contact.ID,
                                                           .nameID          = contact.nameID,
                                                           .numbersID       = contact.numbersID,
                                                           .ringID          = contact.ringID,
                                                           .addressID       = contact.addressID,
                                                           .type            = rec.contactType,
                                                           .isOnWhitelist   = rec.isOnWhitelist,
                                                           .isOnBlacklist   = rec.isOnBlacklist,
                                                           .isOnFavourites  = rec.isOnFavourites,
                                                           .speedDial       = rec.speeddial,
                                                           .namePrimary     = rec.primaryName,
                                                           .nameAlternative = rec.alternativeName});

    if (!ret)
        return ret;

    ret = contactDB->name.Update(ContactsNameTableRow{.ID              = contact.nameID,
                                                      .contactID       = contact.ID,
                                                      .namePrimary     = rec.primaryName,
                                                      .nameAlternative = rec.alternativeName,
                                                      .favourite       = rec.isOnFavourites});

    if (!ret)
        return ret;

    for (auto number : rec.numbers) {
        ret = contactDB->number.Update(ContactsNumberTableRow{.ID = static_cast<uint32_t>(std::stoi(contact.numbersID)),
                                                              .contactID  = contact.ID,
                                                              .numberUser = number.number.getEntered().c_str(),
                                                              .numbere164 = number.number.getE164().c_str(),
                                                              .type       = number.numberType});
    }

    if (!ret)
        return ret;

    ret = contactDB->address.Update(ContactsAddressTableRow{
        .ID = contact.addressID, .contactID = contact.ID, .address = rec.address, .note = rec.note, .mail = rec.mail});

    if (!ret)
        return ret;

    ret = contactDB->ringtones.Update(ContactsRingtonesTableRow{.contactID = contact.ID, .assetPath = rec.assetPath});

    return ret;
}

ContactRecord ContactRecordInterface::GetByID(uint32_t id)
{

    ContactRecord rec = ContactRecord();

    auto contact = contactDB->contacts.GetByID(id);
    if (contact.ID == DB_ID_NONE) {
        return rec;
    }

    std::vector<ContactRecord::Number> nrs = getNumbers(contact.numbersID);
    if (nrs.size() == 0) {
        return rec;
    }
    auto ring = contactDB->ringtones.GetByID(contact.ringID);
    if (ring.ID == DB_ID_NONE) {
        return rec;
    }

    auto address = contactDB->address.GetByID(contact.addressID);
    if (address.ID == DB_ID_NONE) {
        return rec;
    }

    auto name = contactDB->name.GetByID(contact.nameID);
    if (name.ID == 0) {
        return rec;
    }

    rec.ID              = contact.ID;
    rec.primaryName     = name.namePrimary;
    rec.alternativeName = name.nameAlternative;
    rec.numbers         = nrs;
    rec.contactType     = contact.type;
    rec.address         = address.address;
    rec.note            = address.note;
    rec.mail            = address.mail;
    rec.assetPath       = ring.assetPath;
    rec.isOnWhitelist   = contact.isOnWhitelist;
    rec.isOnBlacklist   = contact.isOnBlacklist;
    rec.isOnFavourites  = contact.isOnFavourites;
    rec.speeddial       = contact.speedDial;

    return rec;
}

uint32_t ContactRecordInterface::GetCount()
{

    return contactDB->contacts.GetCount();
}

uint32_t ContactRecordInterface::GetCountFavourites()
{
    return contactDB->contacts.GetCountByFieldID("favourites", 1);
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->name.GetLimitOffset(offset, limit);

    for (const auto &w : ret) {

        auto contact = contactDB->contacts.GetByID(w.ID);
        if (contact.ID == DB_ID_NONE) {
            return records;
        }

        auto name = contactDB->name.GetByID(contact.nameID);
        if (name.ID == DB_ID_NONE) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == DB_ID_NONE) {
            return records;
        }

        auto address = contactDB->address.GetByID(contact.addressID);
        if (address.ID == DB_ID_NONE) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = contact.ID,
                                         .primaryName     = name.namePrimary,
                                         .alternativeName = name.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .isOnWhitelist   = contact.isOnWhitelist,
                                         .isOnBlacklist   = contact.isOnBlacklist,
                                         .isOnFavourites  = contact.isOnFavourites,
                                         .speeddial       = contact.speedDial});
    }

    return records;
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                          uint32_t limit,
                                                                                          ContactRecordField field,
                                                                                          const char *str)
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    switch (field) {
    case ContactRecordField ::PrimaryName: {
        auto ret = contactDB->name.GetLimitOffsetByField(offset, limit, ContactNameTableFields::NamePrimary, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.contactID);
            if (contact.ID == DB_ID_NONE) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == DB_ID_NONE) {
                return records;
            }

            auto address = contactDB->address.GetByID(contact.addressID);
            if (address.ID == DB_ID_NONE) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = w.ID,
                                             .primaryName     = w.namePrimary,
                                             .alternativeName = w.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial});
        }
    } break;

    case ContactRecordField::NumberUser: {
        auto ret = contactDB->number.GetLimitOffsetByField(offset, limit, ContactNumberTableFields ::NumberUser, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.contactID);
            if (contact.ID == DB_ID_NONE) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == DB_ID_NONE) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == DB_ID_NONE) {
                return records;
            }

            auto address = contactDB->address.GetByID(contact.addressID);
            if (address.ID == DB_ID_NONE) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = w.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial

            });
        }
    } break;

    case ContactRecordField::NumberE164: {
        auto ret = contactDB->number.GetLimitOffsetByField(offset, limit, ContactNumberTableFields::NumberE164, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.contactID);
            if (contact.ID == DB_ID_NONE) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == DB_ID_NONE) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == DB_ID_NONE) {
                return records;
            }

            auto address = contactDB->address.GetByID(contact.addressID);
            if (address.ID == DB_ID_NONE) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = w.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial

            });
        }
    } break;
    case ContactRecordField::SpeedDial: {
        auto ret = contactDB->contacts.GetLimitOffsetByField(0, 1, ContactTableFields::SpeedDial, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.ID);
            if (contact.ID == DB_ID_NONE) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == DB_ID_NONE) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == DB_ID_NONE) {
                return records;
            }

            auto address = contactDB->address.GetByID(contact.addressID);
            if (address.ID == DB_ID_NONE) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = contact.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial});
        }
    } break;

    case ContactRecordField::Favourite: {
        auto ret = contactDB->name.GetLimitOffsetByField(offset, limit, ContactNameTableFields::Favourite, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.ID);
            if (contact.ID == DB_ID_NONE) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == DB_ID_NONE) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == DB_ID_NONE) {
                return records;
            }

            auto address = contactDB->address.GetByID(contact.addressID);
            if (address.ID == DB_ID_NONE) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = contact.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial});
        }
    } break;
    }

    return records;
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetByName(UTF8 primaryName, UTF8 alternativeName)
{

    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->name.GetByName(primaryName.c_str(), alternativeName.c_str());

    for (const auto &w : ret) {

        auto contact = contactDB->contacts.GetByID(w.contactID);
        if (contact.ID == DB_ID_NONE) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == DB_ID_NONE) {
            return records;
        }

        auto address = contactDB->address.GetByID(contact.addressID);
        if (address.ID == DB_ID_NONE) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = w.ID,
                                         .primaryName     = w.namePrimary,
                                         .alternativeName = w.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .isOnWhitelist   = contact.isOnWhitelist,
                                         .isOnBlacklist   = contact.isOnBlacklist,
                                         .isOnFavourites  = contact.isOnFavourites,
                                         .speeddial       = contact.speedDial});
    }

    return records;
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::Search(const char *primaryName,
                                                                           const char *alternativeName,
                                                                           const char *number)
{
    auto records = std::make_unique<std::vector<ContactRecord>>();

    auto ret = contactDB->contacts.Search(primaryName, alternativeName, number);

    for (const auto &w : ret) {
        auto contact = contactDB->contacts.GetByID(w.ID);
        if (contact.ID == DB_ID_NONE) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == DB_ID_NONE) {
            return records;
        }

        auto address = contactDB->address.GetByID(contact.addressID);
        if (address.ID == DB_ID_NONE) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = w.ID,
                                         .primaryName     = w.namePrimary,
                                         .alternativeName = w.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .address         = address.address,
                                         .note            = address.note,
                                         .mail            = address.mail,
                                         .assetPath       = ring.assetPath,
                                         .isOnWhitelist   = contact.isOnWhitelist,
                                         .isOnBlacklist   = contact.isOnBlacklist,
                                         .isOnFavourites  = contact.isOnFavourites,
                                         .speeddial       = contact.speedDial});
    }

    return records;
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetContactByNumber(const UTF8 &number)
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::NumberUser, number.c_str());
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetByNumber(const UTF8 &number,
                                                                                CreateTempContact createTempContact)
{
    return GetByNumber(utils::PhoneNumber(number.c_str(), utils::country::Id::UNKNOWN).getView(), createTempContact);
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetByNumber(
    const utils::PhoneNumber::View &numberView, CreateTempContact createTempContact)
{
    auto number = numberView.getEntered();
    auto ret    = GetContactByNumber(number);
    if (ret->size() > 0) {
        return ret;
    }

    // Contact not found, create temporary one
    if (createTempContact == CreateTempContact::True) {
        LOG_INFO("Cannot find contact for number %s, creating temporary one", number.c_str());
        if (!Add(ContactRecord{
                .contactType = ContactType::TEMPORARY,
                .numbers     = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
            })) {
            LOG_ERROR("Cannot add contact record");
            return ret;
        }

        ret->push_back(GetByID(contactDB->GetLastInsertRowID()));
    }

    return ret;
}

std::optional<ContactRecord> ContactRecordInterface::MatchByNumber(const utils::PhoneNumber::View &numberView,
                                                                   CreateTempContact createTempContact,
                                                                   utils::PhoneNumber::Match matchLevel)
{
    utils::NumberHolderMatcher<std::vector, ContactNumberHolder> numberMatcher;
    std::vector<ContactNumberHolder> contactNumberHolders;

    auto numbers = getAllNumbers();

    for (const auto &number : numbers) {
        try {
            contactNumberHolders.push_back(ContactNumberHolder(number));
        }
        catch (const utils::PhoneNumber::Error &e) {
            LOG_WARN(
                "Skipping invalid phone number pair: (%s, %s)", number.numberUser.c_str(), number.numbere164.c_str());
            continue;
        }
    }

    numberMatcher = utils::NumberHolderMatcher<std::vector, ContactNumberHolder>(std::cbegin(contactNumberHolders),
                                                                                 std::cend(contactNumberHolders));

    auto matchedNumber = numberMatcher.bestMatch(utils::PhoneNumber(numberView), matchLevel);

    if (matchedNumber == numberMatcher.END) {
        if (createTempContact != CreateTempContact::True) {
            return std::nullopt;
        }

        LOG_INFO("Cannot find contact for number %s, creating temporary one", numberView.getEntered().c_str());
        if (!Add(ContactRecord{
                .contactType = ContactType::TEMPORARY,
                .numbers     = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
            })) {
            LOG_FATAL("Cannot add contact record");
            return std::nullopt;
        }

        return GetByID(contactDB->GetLastInsertRowID());
    }

    return GetByID(matchedNumber->getContactID());
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetBySpeedDial(UTF8 speedDial)
{
    return GetLimitOffsetByField(0, 1, ContactRecordField::SpeedDial, speedDial.c_str());
}

std::vector<ContactRecord::Number> ContactRecordInterface::getNumbers(const std::string &numbers_id)
{
    std::vector<ContactRecord::Number> nrs;
    for (auto nr_str : utils::split(numbers_id, ' ')) {
        auto nr = contactDB->number.GetByID(std::stol(nr_str));
        if (nr.ID == DB_ID_NONE) {
            return nrs;
        }
        try {
            auto &&number = nr.numbere164.empty() ? utils::PhoneNumber(nr.numberUser, utils::country::Id::UNKNOWN)
                                                  : utils::PhoneNumber(nr.numberUser, nr.numbere164);
            nrs.push_back(ContactRecord::Number(number.getView(), nr.type));
        }
        catch (const utils::PhoneNumber::Error &e) {
            LOG_ERROR("Invalid contact's number pair: \"%s\" (\"%s\", \"%s\"). Using user number instead of a pair.",
                      e.what(),
                      nr.numberUser.c_str(),
                      nr.numbere164.c_str());
            nrs.push_back(ContactRecord::Number(
                utils::PhoneNumber(nr.numberUser, utils::country::Id::UNKNOWN).getView(), nr.type));
        }
    }
    return nrs;
}

ContactRecord::Number::Number()
{}

ContactRecord::Number::Number(const std::string &entered, const std::string &e164, ContactNumberType n_type)
    : number(utils::PhoneNumber(entered, e164).getView()), numberType(n_type)
{}

ContactRecord::Number::Number(const utils::PhoneNumber::View &number, ContactNumberType n_type)
    : number(number), numberType(n_type)
{}

std::vector<ContactsNumberTableRow> ContactRecordInterface::getAllNumbers()
{
    static const std::size_t singleDumpSize = 64;
    std::vector<ContactsNumberTableRow> v;

    std::size_t offset       = 0;
    std::size_t numbersCount = contactDB->number.GetCount();
    while (offset < numbersCount) {
        auto singleRead = contactDB->number.GetLimitOffset(offset, singleDumpSize);
        v.insert(std::end(v), std::begin(singleRead), std::end(singleRead));
        offset += singleDumpSize;
    }

    return v;
}

ContactNumberHolder::ContactNumberHolder(const ContactsNumberTableRow &numberRow)
    : row(numberRow), number(row.numbere164.empty() ? utils::PhoneNumber(row.numberUser)
                                                    : utils::PhoneNumber(row.numberUser, row.numbere164))
{}

const utils::PhoneNumber &ContactNumberHolder::getNumber() const
{
    return number;
}

std::uint32_t ContactNumberHolder::getContactID() const
{
    return row.contactID;
}
