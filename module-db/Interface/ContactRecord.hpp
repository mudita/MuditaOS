
/*
 * @file ContactRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTRECORD_HPP
#define PUREPHONE_CONTACTRECORD_HPP

#include "Record.hpp"
#include "../Databases/ContactsDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct ContactRecord{

    uint32_t dbID;
    UTF8 primaryName;
    UTF8 alternativeName;
    UTF8 numberUser;
    UTF8 numberE164;
    ContactType contactType;

    ContactNumberType numberType;

    UTF8 country;
    UTF8 city;
    UTF8 street;
    UTF8 number;
    UTF8 note;
    UTF8 mail;
    ContactAddressType addressType;

    UTF8 assetPath;

    bool isOnWhitelist;
    bool isOnBlacklist;
    bool isOnFavourites;
    uint8_t speeddial;
};

enum class ContactRecordField {
    PrimaryName,
    NumberE164,
};

class ContactRecordInterface : public RecordInterface<ContactRecord, ContactRecordField> {

public:

    ContactRecordInterface();
    ~ContactRecordInterface();

    bool Add(const ContactRecord &rec) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(const ContactRecord &rec) override final;

    ContactRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ContactRecord>>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactRecordField field, const char *str) override final;

private:
    std::unique_ptr<ContactsDB> contactDB;
};


#endif //PUREPHONE_CONTACTRECORD_HPP
