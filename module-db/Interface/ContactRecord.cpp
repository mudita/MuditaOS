
/*
 * @file ContactRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ContactRecord.hpp"

// TODO copied from Profile -> move to some utils.hpp
#include <sstream>
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) { *(result++) = item;
    }
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
/// <- copy end


ContactRecordInterface::ContactRecordInterface(ContactsDB* db) : contactDB(db) {
}

ContactRecordInterface::~ContactRecordInterface() {

}

ContactRecordInterface::VerifyResult ContactRecordInterface::Verify(const ContactRecord & rec)
{
    VerifyResult retval = VerifyResult::VerifySuccess;
    for ( auto element : rec.numbers) {
        ContactsNumberTableRow entry = {
            .contactID = 0,
            .numberUser = element.numberUser.c_str(),
            .numbere164 = element.numberE164.c_str(),
            .type = element.numberType
        };
        if(!contactDB->number.DuplicateVerify(entry)) {
            return VerifyResult::VerifyNumer;
        }
    }
    if(false == contactDB->name.DuplicateVerify(rec.primaryName)) {
        return VerifyResult::VerifyName;
    }
    if(false == contactDB->contacts.DuplicateVerify(rec.speeddial)) {
        return VerifyResult::VerifySpeedDial;
    }
    return retval;
}

bool ContactRecordInterface::Add(const ContactRecord& rec) {

    bool ret = contactDB->contacts.Add(ContactsTableRow{.type=rec.contactType,
            .isOnWhitelist=rec.isOnBlacklist,
            .isOnBlacklist=rec.isOnBlacklist,
            .isOnFavourites=rec.isOnFavourites,
            .speedDial=rec.speeddial});

    if (!ret) {
        return ret;
    }

    uint32_t contactID = contactDB->GetLastInsertRowID();


    ret = contactDB->name.Add(ContactsNameTableRow{
            .contactID =contactID,
            .namePrimary=rec.primaryName,
            .nameAlternative=rec.alternativeName});

    if (!ret) {
        return ret;
    }

    auto contactNameID = contactDB->GetLastInsertRowID();

    for (auto a : rec.numbers) {
        ContactsNumberTableRow entry = {
            .contactID = contactID,
            .numberUser = a.numberUser.c_str(),
            .numbere164 = a.numberE164.c_str(),
            .type = a.numberType
        };
        ret = contactDB->number.Add(entry);
        if (ret != 0) {
            break;
        }
    }

    if (!ret) {
        return ret;
    }

    auto contactNumberID = contactDB->GetLastInsertRowID();

    ret = contactDB->ringtones.Add(ContactsRingtonesTableRow{
            .contactID = contactID,
            .assetPath = rec.assetPath
    });

    if (!ret) {
        return ret;
    }

    auto contactRingID = contactDB->GetLastInsertRowID();

    // TODO: add missing feature: multiple addresses per contact
    ret = contactDB->address.Add(ContactsAddressTableRow{
            .contactID = contactID,
            .country = rec.country,
            .city = rec.city,
            .street = rec.street,
            .number = rec.number,
            .type = rec.addressType,
            .note = rec.note,
            .mail = rec.mail
    });

    if (!ret) {
        return ret;
    }

    auto contactAddressID = contactDB->GetLastInsertRowID();

    ret = contactDB->contacts.Update(ContactsTableRow{
            .ID=contactID,
            .nameID=contactNameID,
            .numbersID=std::to_string(contactNumberID),
            .ringID=contactRingID,
            .addressIDs=std::to_string(contactAddressID),

            .type=rec.contactType,
            .isOnWhitelist=rec.isOnBlacklist,
            .isOnBlacklist=rec.isOnBlacklist,
            .isOnFavourites=rec.isOnFavourites,
            .speedDial=rec.speeddial});

    return ret;
}

bool ContactRecordInterface::RemoveByID(uint32_t id) {

    auto contact =  contactDB->contacts.GetByID(id);
    if(contact.ID == 0){
        return false;
    }

    if(contactDB->name.RemoveByID(contact.nameID) == false){
        return false;
    }

    if(contactDB->address.RemoveByID(std::stoul(contact.addressIDs)) == false){
        return false;
    }

    if(contactDB->number.RemoveByID(std::stoul(contact.numbersID)) == false){
        return false;
    }

    if(contactDB->ringtones.RemoveByID(contact.ringID) == false){
        return false;
    }

    if(contactDB->contacts.RemoveByID(id) == false){
        return false;
    }

    return true;
}

bool ContactRecordInterface::Update(const ContactRecord &rec) {
    // dummy not needed
    return true;
}

ContactRecord ContactRecordInterface::GetByID(uint32_t id) {

    ContactRecord rec = ContactRecord();

    auto contact = contactDB->contacts.GetByID(id);
    if(contact.ID == 0){
        return rec;
    }

    std::vector<ContactRecord::Number> nrs = getNumbers(contact.numbersID);
    if( nrs.size() == 0) {
        return rec;
    }
    auto ring = contactDB->ringtones.GetByID(contact.ringID);
    if(ring.ID == 0){
        return rec;
    }

    auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
    if(address.ID == 0){
        return rec;
    }

    auto name = contactDB->name.GetByID(contact.nameID);
    if(name.ID == 0){
        return rec;
    }

    rec.dbID = contact.ID;
    rec.primaryName = name.namePrimary;
    rec.alternativeName=name.nameAlternative;
    rec.numbers = nrs;
    rec.contactType=contact.type;
    rec.country=address.country;
    rec.city=address.city;
    rec.street=address.street;
    rec.number=address.number;
    rec.note=address.note;
    rec.mail=address.mail;
    rec.addressType=address.type;
    rec.assetPath=ring.assetPath;
    rec.isOnWhitelist=contact.isOnWhitelist;
    rec.isOnBlacklist=contact.isOnBlacklist;
    rec.isOnFavourites=contact.isOnFavourites;
    rec.speeddial=contact.speedDial;


    return rec;

}

uint32_t ContactRecordInterface::GetCount() {

    return contactDB->contacts.GetCount();
}

void ContactRecordInterface::contact_from_contacts_records(std::vector<ContactRecord> *records, std::vector<ContactsTableRow> &contacts) {
    for(const auto &w : contacts){

        auto nrs = getNumbers(w.numbersID);
        if(nrs.size() == 0){
            return;
        }

        auto ring = contactDB->ringtones.GetByID(w.ringID);
        if(ring.ID == 0){
            return;
        }

        auto address = contactDB->address.GetByID(std::stoul(w.addressIDs));
        if(address.ID == 0){
            return;
        }

        auto name = contactDB->name.GetByID(w.nameID);
        if(name.ID == 0){
            return;
        }
        records->push_back(ContactRecord{
            .dbID = w.ID,
            .primaryName = name.namePrimary,
            .alternativeName=name.nameAlternative,
            .contactType=w.type,
            .numbers = nrs,
            .country=address.country,
            .city=address.city,
            .street=address.street,
            .number=address.number,
            .note=address.note,
            .mail=address.mail,
            .addressType=address.type,
            .assetPath=ring.assetPath,
            .isOnWhitelist=w.isOnWhitelist,
            .isOnBlacklist=w.isOnBlacklist,
            .isOnFavourites=w.isOnFavourites,
            .speeddial=static_cast<uint8_t >(w.speedDial)
        });
    }
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto records =  std::make_unique<std::vector<ContactRecord>>();
    auto contacts = contactDB->contacts.GetLimitOffset(offset,limit);
    contact_from_contacts_records(records.get(), contacts);
    return records;
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                          uint32_t limit,
                                                                                          ContactRecordField field,
                                                                                          const char *str) {
    auto records =  std::make_unique<std::vector<ContactRecord>>();

    switch(field){
        case ContactRecordField ::PrimaryName:
        {
            auto ret = contactDB->name.GetLimitOffsetByField(offset,limit,ContactNameTableFields::NamePrimary,str);

            for(const auto &w : ret){

                auto contact = contactDB->contacts.GetByID(w.contactID);
                if(contact.ID == 0){
                    return records;
                }

                auto nrs = getNumbers(contact.numbersID);
                if(nrs.size() == 0){
                    return records;
                }

                auto ring = contactDB->ringtones.GetByID(contact.ringID);
                if(ring.ID == 0){
                    return records;
                }

                auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
                if(address.ID == 0){
                    return records;
                }


                records->push_back(ContactRecord{
                        .dbID = w.ID,
                        .primaryName = w.namePrimary,
                        .alternativeName=w.nameAlternative,
                        .contactType=contact.type,
                        .numbers = nrs,
                        .country=address.country,
                        .city=address.city,
                        .street=address.street,
                        .number=address.number,
                        .note=address.note,
                        .mail=address.mail,
                        .addressType=address.type,
                        .assetPath=ring.assetPath,
                        .isOnWhitelist=contact.isOnWhitelist,
                        .isOnBlacklist=contact.isOnBlacklist,
                        .isOnFavourites=contact.isOnFavourites,
                        .speeddial=static_cast<uint8_t >(contact.speedDial)
                });
            }


        }

            break;

        case ContactRecordField ::NumberE164:
        {
            auto ret = contactDB->number.GetLimitOffsetByField(offset,limit,ContactNumberTableFields ::NumberE164,str);

            for(const auto &w : ret){

                auto contact = contactDB->contacts.GetByID(w.contactID);
                if(contact.ID == 0){
                    return records;
                }

                auto nrs = getNumbers(contact.numbersID);
                if(nrs.size() == 0){
                    return records;
                }

                auto name = contactDB->name.GetByID(contact.nameID);
                if(name.ID == 0){
                    return records;
                }

                auto ring = contactDB->ringtones.GetByID(contact.ringID);
                if(ring.ID == 0){
                    return records;
                }

                auto address = contactDB->address.GetByID(std::stoul(contact.addressIDs));
                if(address.ID == 0){
                    return records;
                }


                records->push_back(ContactRecord{
                        .dbID = w.ID,
                        .primaryName = name.namePrimary,
                        .alternativeName=name.nameAlternative,
                        .contactType=contact.type,
                        .numbers = nrs,
                        .country=address.country,
                        .city=address.city,
                        .street=address.street,
                        .number=address.number,
                        .note=address.note,
                        .mail=address.mail,
                        .addressType=address.type,
                        .assetPath=ring.assetPath,
                        .isOnWhitelist=contact.isOnWhitelist,
                        .isOnBlacklist=contact.isOnBlacklist,
                        .isOnFavourites=contact.isOnFavourites,
                        .speeddial=static_cast<uint8_t >(contact.speedDial)


                });
            }
        }
        break;
        case ContactRecordField::Favourites:
        {
            records = getFavourites(offset, limit, str);
            break;
        }
    }

    return records;
}

std::vector<ContactRecord::Number> ContactRecordInterface::getNumbers(const std::string &numbers_id)
{
    std::vector<ContactRecord::Number> nrs;
    for (auto nr_str : split(numbers_id, ' ')) {
        auto nr = contactDB->number.GetByID(std::stol(nr_str));
        if(nr.ID == 0) {
            return nrs;
        }
        nrs.push_back(ContactRecord::Number(nr.numberUser, nr.numbere164, nr.type));
    }
    return nrs;
}


std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::getFavourites(uint32_t offset, uint32_t limit, const char* str) {
    auto records =  std::make_unique<std::vector<ContactRecord>>();
    auto contacts = contactDB->contacts.GetLimitOffsetByField(offset, limit, ContactTableFields::Favourites, str);
    contact_from_contacts_records(records.get(), contacts);
    return records;
}
