
/*
 * @file ContactRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once

#include "../Common/Common.hpp"
#include "../Databases/ContactsDB.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"

#include <vector>

struct ContactRecord
{
    uint32_t ID             = DB_ID_NONE;
    UTF8 primaryName        = "";
    UTF8 alternativeName    = "";
    ContactType contactType = ContactType::TEMPORARY;

    struct Number
    {
        UTF8 numberUser              = "";
        UTF8 numberE164              = "";
        ContactNumberType numberType = ContactNumberType::OTHER;
        Number(UTF8 n_user = "", UTF8 n_e164 = "", ContactNumberType n_type = ContactNumberType::CELL)
            : numberUser(n_user), numberE164(n_e164), numberType(n_type)
        {}
    };
    std::vector<Number> numbers;

    UTF8 country                   = "";
    UTF8 city                      = "";
    UTF8 street                    = "";
    UTF8 number                    = "";
    UTF8 note                      = "";
    UTF8 mail                      = "";
    ContactAddressType addressType = ContactAddressType::OTHER;

    UTF8 assetPath = "";

    bool isOnWhitelist  = false;
    bool isOnBlacklist  = false;
    bool isOnFavourites = false;
    UTF8 speeddial      = "";

    inline UTF8 getFormattedName() const
    {
        if (contactType == ContactType::TEMPORARY) {
            return numbers[0].numberE164;
        }

        return primaryName + " " + alternativeName;
    }
};

enum class ContactRecordField
{
    PrimaryName,
    NumberE164,
    SpeedDial,
    Favourite,
};

class ContactRecordInterface : public RecordInterface<ContactRecord, ContactRecordField>
{

  public:
    ContactRecordInterface(ContactsDB *db);
    ~ContactRecordInterface();

    bool Add(const ContactRecord &rec) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(const ContactRecord &rec) override final;

    bool BlockByID(uint32_t id, const bool shouldBeBlocked = true);

    ContactRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountFavourites();

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                      uint32_t limit,
                                                                      ContactRecordField field,
                                                                      const char *str) override final;

    std::unique_ptr<std::vector<ContactRecord>> GetByName(UTF8 primaryName, UTF8 alternativeName);

    enum class CreateTempContact : bool
    {
        False,
        True
    };

    std::unique_ptr<std::vector<ContactRecord>> GetByNumber(
        const UTF8 &number, CreateTempContact createTempContact = CreateTempContact::False);

    std::unique_ptr<std::vector<ContactRecord>> GetBySpeedDial(UTF8 speedDial);

    std::unique_ptr<std::vector<ContactRecord>> Search(const char *primaryName,
                                                       const char *alternativeName,
                                                       const char *number);

  private:
    ContactsDB *contactDB;
    /// get multiple numbers by split numbers_id
    std::vector<ContactRecord::Number> getNumbers(const std::string &numbers_id);

    std::unique_ptr<std::vector<ContactRecord>> GetContactByNumber(const UTF8 &number);
};
