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
    bool ret = contactDB->contacts.add(ContactsTableRow{{.ID = DB_ID_NONE},
                                                        .type           = rec.contactType,
                                                        .speedDial      = rec.speeddial,
                                                        .namePrimary    = rec.primaryName});

    if (!ret) {
        return ret;
    }

    uint32_t contactID = contactDB->getLastInsertRowId();
    LOG_DEBUG("New contact with ID %" PRIu32 " created", contactID);

    ret = contactDB->name.add(ContactsNameTableRow{{.ID = DB_ID_NONE},
                                                   .contactID       = contactID,
                                                   .namePrimary     = rec.primaryName,
                                                   .nameAlternative = rec.alternativeName});

    if (!ret) {
        return ret;
    }

    auto contactNameID = contactDB->getLastInsertRowId();

    for (auto a : rec.numbers) {
        ret = contactDB->number.add(ContactsNumberTableRow{{.ID = DB_ID_NONE},
                                                           .contactID  = contactID,
                                                           .numberUser = a.number.getEntered().c_str(),
                                                           .numbere164 = a.number.getE164().c_str(),
                                                           .type       = a.numberType});
    }

    if (!ret) {
        return ret;
    }

    auto contactNumberID = contactDB->getLastInsertRowId();

    ret = contactDB->ringtones.add(
        ContactsRingtonesTableRow{.ID = DB_ID_NONE, .contactID = contactID, .assetPath = rec.assetPath});

    if (!ret) {
        return ret;
    }

    auto contactRingID = contactDB->getLastInsertRowId();

    ret = contactDB->address.add(ContactsAddressTableRow{
        {.ID = DB_ID_NONE}, .contactID = contactID, .address = rec.address, .note = rec.note, .mail = rec.mail});

    if (!ret) {
        return ret;
    }

    auto contactAddressID = contactDB->getLastInsertRowId();

    ret = contactDB->contacts.update(ContactsTableRow{{.ID = contactID},
                                                      .nameID    = contactNameID,
                                                      .numbersID = std::to_string(contactNumberID),
                                                      .ringID    = contactRingID,
                                                      .addressID = contactAddressID,

                                                      .type           = rec.contactType,
                                                      .speedDial      = rec.speeddial});
    for (auto group : rec.groups) {
        contactDB->groups.addContactToGroup(contactID, group.ID);
    }
    return ret;
}

bool ContactRecordInterface::BlockByID(uint32_t id, const bool shouldBeBlocked)
{
    return contactDB->contacts.BlockByID(id, shouldBeBlocked);
}

bool ContactRecordInterface::RemoveByID(uint32_t id)
{

    auto contact = contactDB->contacts.getById(id);
    if (contact.ID == 0) {
        return false;
    }

    if (contactDB->name.removeById(contact.nameID) == false) {
        return false;
    }

    if (contactDB->address.removeById(contact.addressID) == false) {
        return false;
    }

    if (contactDB->number.removeById(std::stoul(contact.numbersID)) == false) {
        return false;
    }

    if (contactDB->ringtones.removeById(contact.ringID) == false) {
        return false;
    }

    if (contactDB->contacts.removeById(id) == false) {
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
            count = contactDB->contacts.count();
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
    ContactsTableRow contact = contactDB->contacts.getById(rec.ID);
    if (!contact.isValid())
        return false;

    bool ret = contactDB->contacts.update(ContactsTableRow{{.ID = contact.ID},
                                                           .nameID          = contact.nameID,
                                                           .numbersID       = contact.numbersID,
                                                           .ringID          = contact.ringID,
                                                           .addressID       = contact.addressID,
                                                           .type            = rec.contactType,
                                                           .speedDial       = rec.speeddial,
                                                           .namePrimary     = rec.primaryName,
                                                           .nameAlternative = rec.alternativeName});

    if (!ret)
        return ret;

    ret = contactDB->name.update(ContactsNameTableRow{{.ID = contact.nameID},
                                                      .contactID       = contact.ID,
                                                      .namePrimary     = rec.primaryName,
                                                      .nameAlternative = rec.alternativeName});

    if (!ret)
        return ret;

    for (auto number : rec.numbers) {
        ret =
            contactDB->number.update(ContactsNumberTableRow{{.ID = static_cast<uint32_t>(std::stoi(contact.numbersID))},
                                                            .contactID  = contact.ID,
                                                            .numberUser = number.number.getEntered().c_str(),
                                                            .numbere164 = number.number.getE164().c_str(),
                                                            .type       = number.numberType});
    }

    if (!ret)
        return ret;

    ret = contactDB->address.update(ContactsAddressTableRow{{.ID = contact.addressID},
                                                            .contactID = contact.ID,
                                                            .address   = rec.address,
                                                            .note      = rec.note,
                                                            .mail      = rec.mail});

    if (!ret)
        return ret;

    ret = contactDB->ringtones.update(ContactsRingtonesTableRow{.contactID = contact.ID, .assetPath = rec.assetPath});

    if (!ret)
        return ret;

    contactDB->groups.updateGroups(rec.ID, rec.groups);

    return ret;
}

ContactRecord ContactRecordInterface::GetByID(uint32_t id)
{
    ContactRecord rec = ContactRecord();

    auto contact = contactDB->contacts.getById(id);
    if (!contact.isValid()) {
        return rec;
    }

    std::vector<ContactRecord::Number> nrs = getNumbers(contact.numbersID);
    if (nrs.size() == 0) {
        return rec;
    }
    auto ring = contactDB->ringtones.getById(contact.ringID);
    if (!ring.isValid()) {
        return rec;
    }

    auto address = contactDB->address.getById(contact.addressID);
    if (!address.isValid()) {
        return rec;
    }

    auto name = contactDB->name.getById(contact.nameID);
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
    rec.speeddial       = contact.speedDial;
    rec.groups          = contactDB->groups.getGroupsForContact(contact.ID);

    return rec;
}

uint32_t ContactRecordInterface::GetCount()
{
    return contactDB->contacts.count();
}

uint32_t ContactRecordInterface::GetCountFavourites()
{
    return contactDB->contacts.countByFieldId("favourites", 1);
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
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

        records->push_back(ContactRecord{{.ID = contact.ID},
                                         .primaryName     = name.namePrimary,
                                         .alternativeName = name.nameAlternative,
                                         .contactType     = contact.type,
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

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                          uint32_t limit,
                                                                                          ContactRecordField field,
                                                                                          const char *str)
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
                                             .primaryName     = record.namePrimary,
                                             .alternativeName = record.nameAlternative,
                                             .contactType     = contact.type,
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
                return records;
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
                                             .contactType     = contact.type,
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
                return records;
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
                .contactType     = contact.type,
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

            records->push_back(ContactRecord{{.ID = contact.ID},
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .address         = address.address,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .speeddial       = contact.speedDial,
                                             .groups          = contactDB->groups.getGroupsForContact(contact.ID)});
        }
    } break;

    case ContactRecordField::Favourite: {
        auto ret = contactDB->name.getLimitOffsetByField(offset, limit, ContactNameTableFields::Favourite, str);

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

            records->push_back(ContactRecord{{.ID = contact.ID},
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
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

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetByName(UTF8 primaryName, UTF8 alternativeName)
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
                                         .primaryName     = record.namePrimary,
                                         .alternativeName = record.nameAlternative,
                                         .contactType     = contact.type,
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

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::Search(const char *primaryName,
                                                                           const char *alternativeName,
                                                                           const char *number)
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
                                         .primaryName     = record.namePrimary,
                                         .alternativeName = record.nameAlternative,
                                         .contactType     = contact.type,
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
                {.ID = DB_ID_NONE},
                .contactType = ContactType::TEMPORARY,
                .numbers     = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
            })) {
            LOG_ERROR("Cannot add contact record");
            return ret;
        }

        ret->push_back(GetByID(contactDB->getLastInsertRowId()));
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
                {.ID = DB_ID_NONE},
                .contactType = ContactType::TEMPORARY,
                .numbers     = std::vector<ContactRecord::Number>{ContactRecord::Number(numberView)},
            })) {
            LOG_FATAL("Cannot add contact record");
            return std::nullopt;
        }

        return GetByID(contactDB->getLastInsertRowId());
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
        auto nr = contactDB->number.getById(std::stol(nr_str));
        if (!nr.isValid()) {
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
    std::size_t numbersCount = contactDB->number.count();
    while (offset < numbersCount) {
        auto singleRead = contactDB->number.getLimitOffset(offset, singleDumpSize);
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

bool ContactRecord::isOnFavourites()
{
    return isOnGroup(ContactsDB::favouritesGroupId());
}

bool ContactRecord::isOnIce()
{
    return isOnGroup(ContactsDB::iceGroupId());
}

bool ContactRecord::isOnBlocked()
{
    return isOnGroup(ContactsDB::blockedGroupId());
}

bool ContactRecord::isOnGroup(uint32_t groupId)
{
    return groups.find(groupId) != groups.end();
}
