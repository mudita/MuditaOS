#pragma once

#include <Databases/ContactsDB.hpp>
#include <Common/Query.hpp>
#include <Tables/ContactsGroups.hpp>

#include "i18/i18.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"

#include <PhoneNumber.hpp>
#include <NumberHolderMatcher.hpp>
#include "module-gui/gui/widgets/TextConstants.hpp"

#include <memory>
#include <optional>
#include <set>

struct ContactRecord : public Record
{
    UTF8 primaryName     = "";
    UTF8 alternativeName = "";

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
        NotUseNumber,
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
        if (isTemporary()) {
            LOG_DEBUG("temporary contact, numer as name: '%s'", getNumberAsName().c_str());
            return getNumberAsName();
        }
        if (primaryName.length() > 0) {
            return alternativeName.length() > 0 ? primaryName + " " + alternativeName : primaryName;
        }
        if (alternativeName.length() > 0) {
            return alternativeName;
        }
        if (type == NameFormatType::NotUseNumber) {
            return "";
        }
        if ((type == NameFormatType::Default || type == NameFormatType::List) && getNumberAsName().length() > 0) {
            return getNumberAsName();
        }
        if (type == NameFormatType::List || type == NameFormatType::Title) {
            return utils::localize.get("app_phonebook_contact_no_name");
        }
        return "";
    }

    auto getAsString() const -> std::string
    {
        std::stringstream contactData;

        if (getFormattedName(NameFormatType::NotUseNumber).length() > 0) {
            contactData << getFormattedName(NameFormatType::NotUseNumber) << gui::text::newline;
        }
        if (numbers.size() > 0 && numbers[0].number.getEntered().size() > 0) {
            contactData << numbers[0].number.getFormatted() << gui::text::newline;
        }
        if (numbers.size() > 1 && numbers[1].number.getEntered().size() > 0) {
            contactData << numbers[1].number.getFormatted() << gui::text::newline;
        }
        if (mail.length() > 0) {
            contactData << mail << gui::text::newline;
        }
        if (address.length() > 0) {
            contactData << address << gui::text::newline;
        }
        if (note.length() > 0) {
            contactData << note.c_str();
        }

        return contactData.str();
    }

    void addToFavourites(bool add);
    void addToIce(bool add);
    void addToBlocked(bool add);
    void addToGroup(uint32_t groupId);
    void removeFromGroup(uint32_t groupId);
    [[nodiscard]] auto isOnFavourites() const -> bool;
    [[nodiscard]] auto isOnIce() const -> bool;
    [[nodiscard]] auto isOnBlocked() const -> bool;
    [[nodiscard]] auto isOnGroup(uint32_t groupId) const -> bool;
    [[nodiscard]] auto isTemporary() const -> bool;
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
    ContactNumberHolder(ContactsNumberTableRow numberRow);

    auto getNumber() const -> const utils::PhoneNumber &;
    auto getContactID() const -> std::uint32_t;
    auto getNumberID() const -> std::uint32_t;
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
    ~ContactRecordInterface() override = default;

    auto Add(ContactRecord &rec) -> bool final;

    auto RemoveByID(uint32_t id) -> bool final;

    auto Update(const ContactRecord &rec) -> bool final;

    auto BlockByID(uint32_t id, const bool shouldBeBlocked = true) -> bool;

    auto GetByID(uint32_t id) -> ContactRecord final;
    auto GetByIdWithTemporary(uint32_t id) -> ContactRecord;

    auto GetCount() -> uint32_t final;

    auto GetCountFavourites() -> uint32_t;

    auto GetLimitOffset(uint32_t offset, uint32_t limit) -> std::unique_ptr<std::vector<ContactRecord>> final;

    auto GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactRecordField field, const char *str)
        -> std::unique_ptr<std::vector<ContactRecord>> final;

    auto GetByName(UTF8 primaryName, UTF8 alternativeName) -> std::unique_ptr<std::vector<ContactRecord>>;

    enum class CreateTempContact : bool
    {
        False,
        True
    };

    auto GetByNumber(const UTF8 &number, CreateTempContact createTempContact = CreateTempContact::False)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    auto GetByNumber(const utils::PhoneNumber::View &numberView,
                     CreateTempContact createTempContact = CreateTempContact::False)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    auto MatchByNumber(const utils::PhoneNumber::View &numberView,
                       CreateTempContact createTempContact  = CreateTempContact::False,
                       utils::PhoneNumber::Match matchLevel = utils::PhoneNumber::Match::POSSIBLE)
        -> std::optional<ContactNumberMatch>;

    auto GetBySpeedDial(UTF8 speedDial) -> std::unique_ptr<std::vector<ContactRecord>>;

    auto Search(const char *primaryName, const char *alternativeName, const char *number)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult> override;

    auto GetNumberById(std::uint32_t numberId) -> utils::PhoneNumber::View;

  private:
    ContactsDB *contactDB;

    /// get multiple numbers by split numbers_id
    auto getNumbers(const std::string &numbers_id) -> std::vector<ContactRecord::Number>;
    auto getByIdCommon(ContactsTableRow &contact) -> ContactRecord;
    auto getContactByNumber(const UTF8 &number) -> std::unique_ptr<std::vector<ContactRecord>>;
    auto getAllNumbers() -> std::vector<ContactsNumberTableRow>;
    auto buildNumberMatcher(std::vector<ContactNumberHolder> &contactNumberHolders)
        -> utils::NumberHolderMatcher<std::vector, ContactNumberHolder>;
    auto splitNumberIDs(const std::string &numberIDs) -> std::vector<std::uint32_t>;
    auto joinNumberIDs(const std::vector<std::uint32_t> &numberIDs) -> std::string;
    auto unbindNumber(std::uint32_t contactId, std::uint32_t numberId) -> bool;

    const std::uint32_t favouritesGroupId;
    auto getQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto getByIDQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto getSizeQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto addQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto updateQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto removeQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    auto numberGetByIdQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
};
