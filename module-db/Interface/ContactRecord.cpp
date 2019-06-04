
/*
 * @file ContactRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ContactRecord.hpp"

#include "../Databases/ContactsDB.hpp"


bool ContactRecordInterface::Add(const ContactRecord& rec) {
    auto contactDB = std::make_unique<ContactsDB>();


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


    // TODO: add missing feature: multiple numbers per contact
    ret = contactDB->number.Add(ContactsNumberTableRow{
            .contactID = contactID,
            .numberUser = rec.numberUser.c_str(),
            .numbere164 = rec.numberE164.c_str(),
            .type = rec.numberType});

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
    auto contactDB = std::make_unique<ContactsDB>();

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

    auto contactDB = std::make_unique<ContactsDB>();
    ContactRecord rec;

    auto contact = contactDB->contacts.GetByID(id);
    if(contact.ID == 0){
        return rec;
    }

    auto nr = contactDB->number.GetByID(std::stoul(contact.numbersID));
    if(nr.ID == 0){
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
    rec.numberUser=nr.numberUser;
    rec.numberE164=nr.numbere164;
    rec.contactType=contact.type;
    rec.numberType=nr.type;
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
    auto contactDB = std::make_unique<ContactsDB>();

    return contactDB->contacts.GetCount();
}

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto records =  std::make_unique<std::vector<ContactRecord>>();

    auto contactDB = std::make_unique<ContactsDB>();

    auto ret = contactDB->contacts.GetLimitOffset(offset,limit);

    for(const auto &w : ret){

        auto nr = contactDB->number.GetByID(std::stoul(w.numbersID));
        if(nr.ID == 0){
            return records;
        }

        auto ring = contactDB->ringtones.GetByID(w.ringID);
        if(ring.ID == 0){
            return records;
        }

        auto address = contactDB->address.GetByID(std::stoul(w.addressIDs));
        if(address.ID == 0){
            return records;
        }

        auto name = contactDB->name.GetByID(w.nameID);
        if(name.ID == 0){
            return records;
        }

        records->push_back(ContactRecord{
            .dbID = w.ID,
            .primaryName = name.namePrimary,
            .alternativeName=name.nameAlternative,
            .numberUser=nr.numberUser,
            .numberE164=nr.numbere164,
            .contactType=w.type,
            .numberType=nr.type,
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

std::unique_ptr<std::vector<ContactRecord>> ContactRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                          uint32_t limit,
                                                                                          ContactRecordField field,
                                                                                          const char *str) {
    auto records =  std::make_unique<std::vector<ContactRecord>>();
    auto contactDB = std::make_unique<ContactsDB>();

    switch(field){
        case ContactRecordField ::PrimaryName:
        {
            auto ret = contactDB->name.GetLimitOffsetByField(offset,limit,ContactNameTableFields::NamePrimary,str);

            for(const auto &w : ret){

                auto contact = contactDB->contacts.GetByID(w.contactID);
                if(contact.ID == 0){
                    return records;
                }

                auto nr = contactDB->number.GetByID(std::stoul(contact.numbersID));
                if(nr.ID == 0){
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
                        .numberUser=nr.numberUser,
                        .numberE164=nr.numbere164,
                        .contactType=contact.type,
                        .numberType=nr.type,
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
                        .numberUser=w.numberUser,
                        .numberE164=w.numbere164,
                        .contactType=contact.type,
                        .numberType=w.type,
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
    }

    return records;
}