
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
	SpeedDial,
	Favourite,
};

class ContactRecordInterface : public RecordInterface<ContactRecord, ContactRecordField> {

public:

    ContactRecordInterface(ContactsDB* db);
    ~ContactRecordInterface();

    bool Add(const ContactRecord &rec) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(const ContactRecord &rec) override final;

    ContactRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountFavourites();

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ContactRecord>>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactRecordField field, const char *str) override final;

    std::unique_ptr<std::vector<ContactRecord>>
	GetByName( UTF8 primaryName, UTF8 alternativeName );

    std::unique_ptr<std::vector<ContactRecord>>
	GetByNumber( UTF8 number );

    std::unique_ptr<std::vector<ContactRecord>>
	GetBySpeedDial( uint8_t speedDial );

    std::unique_ptr<std::vector<ContactRecord>>
	SearchByName(UTF8 primaryName, UTF8 alternativeName);
private:
    ContactsDB* contactDB;

    /// get multiple numbers by split numbers_id
    std::vector<ContactRecord::Number> getNumbers(const std::string &numbers_id);
};


#endif //PUREPHONE_CONTACTRECORD_HPP
