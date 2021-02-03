// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BaseInterface.hpp>
#include <Common/Common.hpp>
#include <Common/Query.hpp>
#include <Interface/CalllogRecord.hpp>
#include <Interface/ContactRecord.hpp>
#include <Interface/NotesRecord.hpp>
#include <Interface/SMSTemplateRecord.hpp>
#include <Interface/ThreadRecord.hpp>
#include <PhoneNumber.hpp>
#include <Service/Message.hpp>
#include <utf8/UTF8.hpp>
#include <module-db/queries/messages/sms/QuerySMSAdd.hpp>
#include <module-db/Interface/SMSRecord.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

class DBContactMessage;
namespace db
{
    class Query;
} // namespace db
namespace sys
{
    class Service;
} // namespace sys
struct ContactRecord;
struct NotesRecord;

class DBServiceAPI
{
  public:
    static constexpr unsigned int DefaultTimeoutInMs = 5000U;

    enum class ContactVerificationResult
    {
        emptyContact,
        temporaryContactExists,
        primaryNumberDuplicate,
        secondaryNumberDuplicate,
        speedDialDuplicate,
        success
    };

    static auto ThreadGetByNumber(sys::Service *serv,
                                  const utils::PhoneNumber::View &phoneNumber,
                                  std::uint32_t timeout = DefaultTimeoutInMs) -> std::unique_ptr<ThreadRecord>;
    static auto ThreadGetCount(sys::Service *serv, EntryState state = EntryState::ALL) -> uint32_t;

    /**
     * Queries the database.
     * @param serv      Sender service.
     * @param database  Target database name.
     * @param query     Query.
     * @return A pair of: a flag that indicates whether query send was successful, and a message identifier that common
     * for the query and its response.
     */
    static auto GetQuery(sys::Service *serv, db::Interface::Name database, std::unique_ptr<db::Query> query)
        -> std::pair<bool, std::uint64_t>;
    static auto GetQueryWithReply(sys::Service *serv,
                                  db::Interface::Name database,
                                  std::unique_ptr<db::Query> query,
                                  std::uint32_t timeout) -> sys::SendResult;

    /**
     * @brief Function is checking if new contact can be added to database. Function is blocking.
     * @param serv Pointer to Service based object that is sending request.
     * @param rec Reference to the contact to be added to database.
     * @param errNumPrim in case of existing contact with same primary phone number this record will be filled with
     * data.
     * @param errNumAlt in case of existing contact with same alternative phone number this record will be filled
     * with data.
     * @param errSpeedDial in case of existing contact with same speed dial assigned this record will be filled with
     * data.
     *
     * @note This function is blocking. It's checking until first error.
     */
    static auto verifyContact(sys::Service *serv, const ContactRecord &rec) -> ContactVerificationResult;
    static auto ContactGetByID(sys::Service *serv, uint32_t contactID) -> std::unique_ptr<std::vector<ContactRecord>>;
    static auto ContactGetByIDWithTemporary(sys::Service *serv, uint32_t contactID)
        -> std::unique_ptr<std::vector<ContactRecord>>;

  private:
    static auto ContactGetByIDCommon(sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg)
        -> std::unique_ptr<std::vector<ContactRecord>>;

  public:
    static auto ContactGetBySpeeddial(sys::Service *serv, UTF8 speeddial)
        -> std::unique_ptr<std::vector<ContactRecord>>;
    static auto ContactGetByPhoneNumber(sys::Service *serv, UTF8 phoneNumber)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    /**
     * @brief Matches single Contact by a provided phone number
     *
     * @param serv - calling service
     * @param numberView - number to match contact with
     * @return std::unique_ptr<ContactRecord>
     */
    static auto MatchContactByPhoneNumber(sys::Service *serv, const utils::PhoneNumber::View &numberView)
        -> std::unique_ptr<ContactRecord>;
    static auto ContactAdd(sys::Service *serv, const ContactRecord &rec) -> bool;
    static auto ContactRemove(sys::Service *serv, uint32_t id) -> bool;
    static auto ContactUpdate(sys::Service *serv, const ContactRecord &rec) -> bool;
    static auto ContactSearch(sys::Service *serv, UTF8 primaryName, UTF8 alternativeName, UTF8 number)
        -> std::unique_ptr<std::vector<ContactRecord>>;

    static auto CalllogAdd(sys::Service *serv, const CalllogRecord &rec) -> CalllogRecord;
    static auto CalllogRemove(sys::Service *serv, uint32_t id) -> bool;
    static auto CalllogUpdate(sys::Service *serv, const CalllogRecord &rec) -> bool;
    static auto CalllogGetCount(sys::Service *serv, EntryState state = EntryState::ALL) -> uint32_t;
    static auto CalllogGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit) -> bool;

    /* country codes */
    static auto GetCountryCodeByMCC(sys::Service *serv, uint32_t mcc) -> uint32_t;

    static auto DBBackup(sys::Service *serv, std::string backupPath) -> bool;

    /**
     * @brief Add sms via DBService interface
     *
     * @param serv - calling service
     * @param record - sms record data
     * @param listener - query listener to obtain and handle query result
     * @return true if adding sms operation succeed, otherwise false
     */
    static bool AddSMS(sys::Service *serv, const SMSRecord &record, std::unique_ptr<db::QueryListener> &&listener);
};
