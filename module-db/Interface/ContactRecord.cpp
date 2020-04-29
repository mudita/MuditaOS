#include "ContactRecord.hpp"
#include <Utils.hpp>
#include <log/log.hpp>

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
                                                           .numberUser = a.numberUser.c_str(),
                                                           .numbere164 = a.numberE164.c_str(),
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

    ret = contactDB->address.Add(ContactsAddressTableRow{.contactID    = contactID,
                                                         .addressLine1 = rec.addressLine1,
                                                         .addressLine2 = rec.addressLine2,
                                                         .note         = rec.note,
                                                         .mail         = rec.mail});

    if (!ret) {
        return ret;
    }

    auto contactAddressID = contactDB->GetLastInsertRowID();

    ret = contactDB->contacts.Update(ContactsTableRow{.ID         = contactID,
                                                      .nameID     = contactNameID,
                                                      .numbersID  = std::to_string(contactNumberID),
                                                      .ringID     = contactRingID,
                                                      .addressIDs = std::to_string(contactAddressID),

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

    if (contactDB->address.RemoveByID(std::stoul(contact.addressIDs)) == false) {
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

bool ContactRecordInterface::Update(const ContactRecord &rec)
{
    ContactsTableRow contact = contactDB->contacts.GetByID(rec.ID);
    if (contact.ID == DB_ID_NONE)
        return false;

    bool ret = contactDB->contacts.Update(ContactsTableRow{.ID              = contact.ID,
                                                           .nameID          = contact.nameID,
                                                           .numbersID       = contact.numbersID,
                                                           .ringID          = contact.ringID,
                                                           .addressIDs      = contact.addressIDs,
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
                                                              .numberUser = number.numberUser.c_str(),
                                                              .numbere164 = number.numberE164.c_str(),
                                                              .type       = number.numberType});
    }

    if (!ret)
        return ret;

    ret = contactDB->address.Update(ContactsAddressTableRow{.contactID    = contact.ID,
                                                            .addressLine1 = rec.addressLine1,
                                                            .addressLine2 = rec.addressLine2,
                                                            .note         = rec.note,
                                                            .mail         = rec.mail});

    if (!ret)
        return ret;

    ret = contactDB->ringtones.Update(ContactsRingtonesTableRow{.contactID = contact.ID, .assetPath = rec.assetPath});

    return ret;
}

ContactRecord ContactRecordInterface::GetByID(uint32_t id)
{

    ContactRecord rec = ContactRecord();

    auto contact = contactDB->contacts.GetByID(id);
    if (contact.ID == 0) {
        return rec;
    }

    std::vector<ContactRecord::Number> nrs = getNumbers(contact.numbersID);
    if (nrs.size() == 0) {
        return rec;
    }
    auto ring = contactDB->ringtones.GetByID(contact.ringID);
    if (ring.ID == 0) {
        return rec;
    }

    auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
    if (address.ID == 0) {
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
    rec.addressLine1    = address.addressLine1;
    rec.addressLine2    = address.addressLine2;
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
        if (contact.ID == 0) {
            return records;
        }

        auto name = contactDB->name.GetByID(contact.nameID);
        if (name.ID == 0) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == 0) {
            return records;
        }

        auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
        if (address.ID == 0) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = contact.ID,
                                         .primaryName     = name.namePrimary,
                                         .alternativeName = name.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .addressLine1    = address.addressLine1,
                                         .addressLine2    = address.addressLine2,
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
            if (contact.ID == 0) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == 0) {
                return records;
            }

            auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
            if (address.ID == 0) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = w.ID,
                                             .primaryName     = w.namePrimary,
                                             .alternativeName = w.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .addressLine1    = address.addressLine1,
                                             .addressLine2    = address.addressLine2,
                                             .note            = address.note,
                                             .mail            = address.mail,
                                             .assetPath       = ring.assetPath,
                                             .isOnWhitelist   = contact.isOnWhitelist,
                                             .isOnBlacklist   = contact.isOnBlacklist,
                                             .isOnFavourites  = contact.isOnFavourites,
                                             .speeddial       = contact.speedDial});
        }
    }

    break;

    case ContactRecordField ::NumberE164: {
        auto ret = contactDB->number.GetLimitOffsetByField(offset, limit, ContactNumberTableFields ::NumberE164, str);

        for (const auto &w : ret) {

            auto contact = contactDB->contacts.GetByID(w.contactID);
            if (contact.ID == 0) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == 0) {
                return records;
            }

            auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
            if (address.ID == 0) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = w.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .addressLine1    = address.addressLine1,
                                             .addressLine2    = address.addressLine2,
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
            if (contact.ID == 0) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == 0) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == 0) {
                return records;
            }

            auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
            if (address.ID == 0) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = contact.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .addressLine1    = address.addressLine1,
                                             .addressLine2    = address.addressLine2,
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
            if (contact.ID == 0) {
                return records;
            }

            auto name = contactDB->name.GetByID(contact.nameID);
            if (name.ID == 0) {
                return records;
            }

            auto nrs = getNumbers(contact.numbersID);
            if (nrs.size() == 0) {
                return records;
            }

            auto ring = contactDB->ringtones.GetByID(contact.ringID);
            if (ring.ID == 0) {
                return records;
            }

            auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
            if (address.ID == 0) {
                return records;
            }

            records->push_back(ContactRecord{.ID              = contact.ID,
                                             .primaryName     = name.namePrimary,
                                             .alternativeName = name.nameAlternative,
                                             .contactType     = contact.type,
                                             .numbers         = nrs,
                                             .addressLine1    = address.addressLine1,
                                             .addressLine2    = address.addressLine2,
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
        if (contact.ID == 0) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == 0) {
            return records;
        }

        auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
        if (address.ID == 0) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = w.ID,
                                         .primaryName     = w.namePrimary,
                                         .alternativeName = w.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .addressLine1    = address.addressLine1,
                                         .addressLine2    = address.addressLine2,
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
        if (contact.ID == 0) {
            return records;
        }

        auto nrs = getNumbers(contact.numbersID);
        if (nrs.size() == 0) {
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(contact.ringID);
        if (ring.ID == 0) {
            return records;
        }

        auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
        if (address.ID == 0) {
            return records;
        }

        records->push_back(ContactRecord{.ID              = w.ID,
                                         .primaryName     = w.namePrimary,
                                         .alternativeName = w.nameAlternative,
                                         .contactType     = contact.type,
                                         .numbers         = nrs,
                                         .addressLine1    = address.addressLine1,
                                         .addressLine2    = address.addressLine2,
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
    return GetLimitOffsetByField(0, 1, ContactRecordField::NumberE164, number.c_str());
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetByNumber(const UTF8 &number,
                                                                                CreateTempContact createTempContact)
{
    auto ret = GetContactByNumber(number);
    if (ret->size() > 0) {
        return ret;
    }

    // Contact not found, create temporary one
    if (createTempContact == CreateTempContact::True) {
        LOG_INFO("Cannot find contact for number %s, creating temporary one", number.c_str());
        if (!Add(ContactRecord{
                .contactType = ContactType::TEMPORARY,
                .numbers = std::vector<ContactRecord::Number>{ContactRecord::Number(number.c_str(), number.c_str())},
            })) {
            LOG_ERROR("Cannot add contact record");
            return ret;
        }

        ret->push_back(GetByID(contactDB->GetLastInsertRowID()));
    }

    return ret;
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
        if (nr.ID == 0) {
            return nrs;
        }
        nrs.push_back(ContactRecord::Number(nr.numberUser, nr.numbere164, nr.type));
    }
    return nrs;
}
