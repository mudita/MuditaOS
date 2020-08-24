#pragma once

#include <Databases/ContactsDB.hpp>
#include <Common/Query.hpp>
#include <Tables/ContactsGroups.hpp>

#include "i18/i18.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"

#include <PhoneNumber.hpp>
#include <NumberHolderMatcher.hpp>

#include <memory>
#include <optional>
#include <set>

struct ContactRecord : public Record
{
    UTF8 primaryName        = "";
    UTF8 alternativeName    = "";

    struct Number
    {
        utils::PhoneNumber::View number;
        ContactNumberType numberType = ContactNumberType::OTHER;
        Number();
        explicit Number(const utils::PhoneNumber::View &number, ContactNumberType = ContactNumberType::CELL);
        explicit Number(const std::string &entered,
                        const std::string &e164,
                        ContactNumberType n_type = ContactNumberType::CELL);
    };
    std::vector<Number> numbers = {};

    UTF8 address                   = "";
    UTF8 note                      = "";
    UTF8 mail                      = "";
    ContactAddressType addressType = ContactAddressType::OTHER;

    UTF8 assetPath = "";

    UTF8 speeddial                          = "";
    std::set<ContactsGroupsTableRow> groups = {};

    enum class NameFormatType
    {
        Default,
        List,
        Title,
    };

    inline auto getNumberAsName() const -> UTF8
    {
        if (numbers.size() > 0 && numbers[0].number.getEntered().size() > 0) {
            return numbers[0].number.getFormatted();
        }
        if (numbers.size() > 1 && numbers[1].number.getEntered().size() > 0) {
            return numbers[1].number.getFormatted();
        }
        return "";
    }

    inline auto getFormattedName(const NameFormatType type = NameFormatType::Default) const -> UTF8
    {
        if (isTemporrary()) {
            LOG_DEBUG("temporary contact, numer as name: '%s'", getNumberAsName().c_str());
            return getNumberAsName();
        }
        if (primaryName.length() > 0) {
            return alternativeName.length() > 0 ? primaryName + " " + alternativeName : primaryName;
        }
        if (alternativeName.length() > 0) {
            return alternativeName;
        }
        if ((type == NameFormatType::Default || type == NameFormatType::List) && getNumberAsName().length() > 0) {
            return getNumberAsName();
        }
        if (type == NameFormatType::List || type == NameFormatType::Title) {
            return utils::localize.get("app_phonebook_contact_no_name");
        }
        return "";
    }

    void addToFavourites(bool add);
    void addToIce(bool add);
    void addToBlocked(bool add);
    void addToGroup(uint32_t groupId);
    void removeFromGroup(uint32_t groupId);
    [[nodiscard]] bool isOnFavourites() const;
    [[nodiscard]] bool isOnIce() const;
    [[nodiscard]] bool isOnBlocked() const;
    [[nodiscard]] bool isOnGroup(uint32_t groupId) const;
    [[nodiscard]] bool isTemporrary() const;
};

enum class ContactRecordField
{
    PrimaryName,
    NumberE164,
    NumberUser,
    SpeedDial,
    Groups,
};

class ContactNumberHolder
{
  private:
    ContactsNumberTableRow row;
    utils::PhoneNumber number;

  public:
    ContactNumberHolder(const ContactsNumberTableRow &numberRow);
    const utils::PhoneNumber &getNumber() const;
    std::uint32_t getContactID() const;
    std::uint32_t getNumberID() const;
};

class ContactRecordInterface : public RecordInterface<ContactRecord, ContactRecordField>
{
  public:
    struct ContactNumberMatch
    {
        ContactRecord contact;
        std::uint32_t contactId = DB_ID_NONE;
        std::uint32_t numberId  = DB_ID_NONE;

        ContactNumberMatch(ContactRecord rec, std::uint32_t contactId, std::uint32_t numberId);
    };

    ContactRecordInterface(ContactsDB *db);
    ~ContactRecordInterface();

    bool Add(ContactRecord &rec) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(const ContactRecord &rec) override final;

    bool BlockByID(uint32_t id, const bool shouldBeBlocked = true);

    ContactRecord GetByID(uint32_t id) override final;
    ContactRecord GetByIdWithTemporary(uint32_t id);

  private:
    ContactRecord GetByIdCommon(ContactsTableRow &contact);

  public:
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

    std::unique_ptr<std::vector<ContactRecord>> GetByNumber(
        const utils::PhoneNumber::View &numberView, CreateTempContact createTempContact = CreateTempContact::False);

    std::optional<ContactNumberMatch> MatchByNumber(
        const utils::PhoneNumber::View &numberView,
        CreateTempContact createTempContact  = CreateTempContact::False,
        utils::PhoneNumber::Match matchLevel = utils::PhoneNumber::Match::POSSIBLE);

    std::unique_ptr<std::vector<ContactRecord>> GetBySpeedDial(UTF8 speedDial);

    std::unique_ptr<std::vector<ContactRecord>> Search(const char *primaryName,
                                                       const char *alternativeName,
                                                       const char *number);

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

    utils::PhoneNumber::View GetNumberById(std::uint32_t numberId);

  private:
    ContactsDB *contactDB;

    /// get multiple numbers by split numbers_id
    std::vector<ContactRecord::Number> getNumbers(const std::string &numbers_id);

    std::unique_ptr<std::vector<ContactRecord>> GetContactByNumber(const UTF8 &number);

    std::vector<ContactsNumberTableRow> getAllNumbers();

    utils::NumberHolderMatcher<std::vector, ContactNumberHolder> buildNumberMatcher(
        std::vector<ContactNumberHolder> &contactNumberHolders);

    std::vector<std::uint32_t> splitNumberIDs(const std::string &numberIDs);

    std::string joinNumberIDs(const std::vector<std::uint32_t> &numberIDs);

    bool unbindNumber(std::uint32_t contactId, std::uint32_t numberId);

    const std::uint32_t favouritesGroupId;
    std::unique_ptr<db::QueryResult> getQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getByIDQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getSizeQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> addQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> updateQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> removeQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> numberGetByIdQuery(std::shared_ptr<db::Query> query);
};
