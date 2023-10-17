// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <Common/Logging.hpp>
#include "module-db/databases/ContactsDB.hpp"
#include <Tables/ContactsGroups.hpp>

#include <i18n/i18n.hpp>
#include "Record.hpp"
#include "utf8/UTF8.hpp"

#include <PhoneNumber.hpp>
#include <NumberHolderMatcher.hpp>
#include "module-gui/gui/widgets/text/TextConstants.hpp"
#include <module-apps/application-phonebook/data/ContactsMap.hpp>

#include <memory>
#include <optional>
#include <set>
#include <sstream>

struct ContactRecord : public Record
{
    UTF8 primaryName          = "";
    UTF8 alternativeName      = "";
    uint32_t contactPosOnList = 0;

    struct Number
    {
        utils::PhoneNumber::View number;
        ContactNumberType numberType = ContactNumberType::OTHER;
        std::uint64_t numberId       = 0;
        Number();
        explicit Number(const utils::PhoneNumber::View &number,
                        ContactNumberType      = ContactNumberType::CELL,
                        const std::uint64_t id = 0);
        explicit Number(const std::string &entered,
                        const std::string &e164,
                        ContactNumberType n_type = ContactNumberType::CELL,
                        const std::uint64_t id   = 0);
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
        if (!numbers.empty() && !numbers[0].number.getEntered().empty()) {
            return numbers[0].number.getFormatted();
        }
        if (numbers.size() > 1 && !numbers[1].number.getEntered().empty()) {
            return numbers[1].number.getFormatted();
        }
        return "";
    }

    inline auto getFormattedName(const NameFormatType type = NameFormatType::Default) const -> UTF8
    {
        if (isTemporary()) {
            debug_db_data("temporary contact, number as name: '%s'", getNumberAsName().c_str());
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
            return utils::translate("app_phonebook_contact_no_name");
        }
        return "";
    }

    auto getAsString() const -> std::string
    {
        std::stringstream contactData;

        if (getFormattedName(NameFormatType::NotUseNumber).length() > 0) {
            contactData << getFormattedName(NameFormatType::NotUseNumber) << gui::text::newline;
        }
        if (!numbers.empty() && !numbers[0].number.getEntered().empty()) {
            contactData << numbers[0].number.getFormatted() << gui::text::newline;
        }
        if (numbers.size() > 1 && !numbers[1].number.getEntered().empty()) {
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
    ContactsNumberTableRow numberRow;

  public:
    ContactNumberHolder(ContactsNumberTableRow &&numberRow);

    auto getNumber() const noexcept -> utils::PhoneNumber;
    auto getContactID() const noexcept -> std::uint32_t;
    auto getNumberID() const noexcept -> std::uint32_t;
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

    explicit ContactRecordInterface(ContactsDB *db);
    ~ContactRecordInterface() override = default;

    auto Add(ContactRecord &rec) -> bool final;

    auto RemoveByID(uint32_t id) -> bool final;

    auto Update(const ContactRecord &rec) -> bool final;

    auto BlockByID(uint32_t id, bool shouldBeBlocked = true) -> bool;

    auto GetByID(uint32_t id) -> ContactRecord final;
    auto GetByIdWithTemporary(uint32_t id) -> ContactRecord;

    auto GetCount() -> uint32_t final;

    auto GetCountFavourites() -> uint32_t;

    auto GetLimitOffset(uint32_t offset, uint32_t limit) -> std::unique_ptr<std::vector<ContactRecord>> final;

    auto GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactRecordField field, const char *str)
        -> std::unique_ptr<std::vector<ContactRecord>> final;

    auto GetByName(const UTF8 &primaryName, const UTF8 &alternativeName) -> std::unique_ptr<std::vector<ContactRecord>>;

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

    auto GetByNumberID(std::uint32_t numberId) -> std::optional<ContactRecord>;

    auto MatchByNumber(const utils::PhoneNumber::View &numberView,
                       CreateTempContact createTempContact   = CreateTempContact::False,
                       utils::PhoneNumber::Match matchLevel  = utils::PhoneNumber::Match::POSSIBLE,
                       const std::uint32_t contactIDToIgnore = 0u) -> std::optional<ContactNumberMatch>;

    auto GetBySpeedDial(const UTF8 &speedDial) -> std::unique_ptr<std::vector<ContactRecord>>;

    auto Search(const char *primaryName, const char *alternativeName, const char *number)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult> override;

    auto GetNumberById(std::uint32_t numberId) -> utils::PhoneNumber::View;

    auto GetNumbersIdsByContact(std::uint32_t contactId) -> std::vector<std::uint32_t>;

    auto hasContactRecordSameNumbers(const ContactRecord &rec) -> bool;

    /**
     * @brief Merge contacts list with overriding the duplicates in contacts DB
     *
     * @param contacts vector of contacts with single number
     * @return boolean status
     */
    auto MergeContactsList(std::vector<ContactRecord> &contacts) -> std::vector<std::pair<db::Query::Type, uint32_t>>;

    /**
     * @brief Check which contacts in vector are duplicating contacts in DB
     *
     * @param contacts vector of contacts with single number
     * @return first vector of contacts with unique numbers and second with duplicates appearing in contacts DB
     */
    auto CheckContactsListDuplicates(std::vector<ContactRecord> &contacts)
        -> std::pair<std::vector<ContactRecord>, std::vector<ContactRecord>>;

    /**
     * @brief Verify if single contact record can be considered as a duplicate in DB
     *
     * @param record single contact record to be verified
     * @return true if contact can be considered as a duplicate in DB
     */
    auto verifyDuplicate(ContactRecord &record) -> std::vector<utils::PhoneNumber::View>;

    /**
     * @brief Verify if single contact record can be considered as an existing temporary contact in DB
     *
     * @param record single contact record to be verified
     * @return true if contact can be considered as already existing in DB, as a temporary contact
     */
    auto verifyTemporary(ContactRecord &record) -> bool;

  private:
    ContactsDB *contactDB = nullptr;

    /// get multiple numbers by split numbersId
    auto getNumbers(const std::string &numbersId) -> std::vector<ContactRecord::Number>;
    auto getByIdCommon(ContactsTableRow &contact) -> ContactRecord;
    auto getContactByNumber(const UTF8 &number) -> const std::unique_ptr<std::vector<ContactRecord>>;
    auto getAllNumbers() -> const std::vector<ContactsNumberTableRow>;
    auto buildNumberMatcher(unsigned int maxPageSize = std::numeric_limits<unsigned int>::max())
        -> utils::NumberHolderMatcher<std::vector, ContactNumberHolder>;
    auto splitNumberIDs(const std::string &numberIDs) -> const std::vector<std::uint32_t>;
    auto joinNumberIDs(const std::vector<std::uint32_t> &numberIDs) -> std::string;
    auto unbindNumber(std::uint32_t contactId, std::uint32_t numberId) -> bool;
    void unbindSpeedDialKeyFromOtherContacts(const UTF8 &key);

    const std::uint32_t favouritesGroupId;
    auto getQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<ContactRecord>;
    auto getQueryWithTotalCount(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto getForListQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto getLetterMapQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;

    auto getByIDQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto getByNumberIDQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto getContactsSize(const std::shared_ptr<db::Query> &query) -> std::size_t;
    auto getSizeQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto addQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto updateQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto removeQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto numberGetByIdQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto mergeContactsListQuery(const std::shared_ptr<db::Query> &query) -> const std::unique_ptr<db::QueryResult>;
    auto checkContactsListDuplicatesQuery(const std::shared_ptr<db::Query> &query)
        -> const std::unique_ptr<db::QueryResult>;

    auto addTemporaryContactForNumber(const ContactRecord::Number &number) -> std::optional<ContactRecord>;
    auto getNumbersIDs(std::uint32_t contactID,
                       const ContactRecord &contact,
                       utils::PhoneNumber::Match matchLevel = utils::PhoneNumber::Match::POSSIBLE)
        -> std::vector<std::uint32_t>;
    auto addNumbers(std::uint32_t contactID, const std::vector<ContactRecord::Number> &numbers)
        -> std::optional<std::string>;
    auto addOrUpdateName(std::uint32_t contactID, std::uint32_t nameID, const ContactRecord &contact)
        -> std::optional<std::uint32_t>;
    auto addOrUpdateAddress(std::uint32_t contactID, std::uint32_t addressID, const ContactRecord &contact)
        -> std::optional<std::uint32_t>;
    auto addOrUpdateRingtone(std::uint32_t contactID, std::uint32_t ringtoneID, const ContactRecord &contact)
        -> std::optional<std::uint32_t>;

    auto matchedNumberRefersToTemporary(const ContactNumberHolder &matchedNumber) -> bool;

    /**
     * @brief Changing number table record in place if new number is same as old number but with/without country
     * code
     *
     * @param oldNumberIDs vector of old number IDs in contact_number table which can be changed in place
     *                     only if in  newNumbers  is same number but with/without country code
     * @param newNumbers  vector of numbers to which we want to change the old ones
     * @return true if operation have no error
     */
    auto changeNumberRecordInPlaceIfCountryCodeIsOnlyDifferent(const std::vector<std::uint32_t> &oldNumberIDs,
                                                               std::vector<ContactRecord::Number> &newNumbers) -> bool;
};
