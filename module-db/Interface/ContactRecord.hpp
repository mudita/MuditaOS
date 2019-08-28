
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

#include <vector>

struct ContactRecord{

    uint32_t dbID;
    UTF8 primaryName;
    UTF8 alternativeName;
    ContactType contactType;

    struct Number {
        UTF8 numberUser;
        UTF8 numberE164;
        ContactNumberType numberType;
        Number(UTF8 n_user="", UTF8 n_e164="", ContactNumberType n_type=ContactNumberType::CELL) :numberUser(n_user), numberE164(n_e164), numberType(n_type) {}
    };
    std::vector<Number> numbers;

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
    Favourites,
};

class ContactRecordInterface : public RecordInterface<ContactRecord, ContactRecordField> {

public:

    ContactRecordInterface(ContactsDB* db);
    ~ContactRecordInterface();

    enum class VerifyResult : uint32_t {
        VerifySuccess =0,
        VerifyDB_Error,
        VerifyNumer,
        VerifyName,
        VerifySpeedDial,
    };

    /// verifies if contact is ok and returns what's wrong if not
    VerifyResult Verify(const ContactRecord & rec);

    bool Add(const ContactRecord &rec) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(const ContactRecord &rec) override final;

    ContactRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ContactRecord>>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactRecordField field, const char *str) override final;
    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffsetLike(uint32_t offset, uint32_t limit, UTF8 text);

private:
    ContactsDB* contactDB;

    /// get multiple numbers by split numbers_id
    std::vector<ContactRecord::Number> getNumbers(const std::string &numbers_id);
    /// get favourite contact list
    std::unique_ptr<std::vector<ContactRecord>> getFavourites(uint32_t offset, uint32_t limit, const char* str);
    /// helper function to avoid needles code copies
    void contact_from_contacts_records(std::vector<ContactRecord> *record, std::vector<ContactsTableRow> &contacts);
    /// helper function
    void build_contact_records_by_name(std::vector<ContactsNameTableRow> &ret, std::vector<ContactRecord> *records);
};


#endif //PUREPHONE_CONTACTRECORD_HPP
