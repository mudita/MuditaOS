﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BaseInterface.hpp>
#include <Common/Common.hpp>
#include <Common/Query.hpp>
#include <Interface/AlarmsRecord.hpp>
#include <Interface/CalllogRecord.hpp>
#include <Interface/ContactRecord.hpp>
#include <Interface/NotesRecord.hpp>
#include <Interface/SMSRecord.hpp>
#include <Interface/SMSTemplateRecord.hpp>
#include <Interface/ThreadRecord.hpp>
#include <PhoneNumber.hpp>
#include <Service/Bus.hpp>
#include <Service/Message.hpp>
#include <utf8/UTF8.hpp>

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

    enum ContactVerificationError
    {
        emptyContactError,
        speedDialError,
        primaryNumberError,
        secondaryNumberError,
        noError
    };

    /**
     * @brief Function is adding new SMS to SMSDB.
     * @param serv Pointer to Service based object that is sending request.
     * @param rec Record to add.
     * @return dbID of added record.
     */
    static uint32_t SMSAdd(sys::Service *serv, const SMSRecord &rec);
    static bool SMSRemove(sys::Service *serv, const SMSRecord &rec);
    static bool SMSUpdate(sys::Service *serv, const SMSRecord &rec);
    static std::unique_ptr<std::vector<SMSRecord>> SMSGetLimitOffset(sys::Service *serv,
                                                                     uint32_t offset,
                                                                     uint32_t limit);
    static std::unique_ptr<std::vector<SMSRecord>> SMSGetLimitOffsetByThreadID(sys::Service *serv,
                                                                               uint32_t offset,
                                                                               uint32_t limit,
                                                                               uint32_t id);
    static uint32_t SMSGetCount(sys::Service *serv);
    /**
     * @brief Function is getting last modified SMS record.
     * @param serv Pointer to Service based object that is sending request.
     * @return record.
     */
    static SMSRecord SMSGetLastRecord(sys::Service *serv);

    static std::unique_ptr<ThreadRecord> ThreadGet(sys::Service *serv, uint32_t id);
    static std::unique_ptr<ThreadRecord> ThreadGetByContact(sys::Service *serv, uint32_t contactID);
    static std::unique_ptr<ThreadRecord> ThreadGetByNumber(sys::Service *serv,
                                                           const utils::PhoneNumber::View &phoneNumber,
                                                           std::uint32_t timeout = DefaultTimeoutInMs);
    static bool ThreadRemove(sys::Service *serv, uint32_t id);
    static bool ThreadGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit);
    static uint32_t ThreadGetCount(sys::Service *serv, EntryState state = EntryState::ALL);
    static bool ThreadUpdate(sys::Service *serv, const ThreadRecord &rec);

    static bool SMSTemplateAdd(sys::Service *serv, const SMSTemplateRecord &rec);
    static bool SMSTemplateRemove(sys::Service *serv, uint32_t id);
    static bool SMSTemplateUpdate(sys::Service *serv, const SMSTemplateRecord &rec);
    static uint32_t SMSTemplateGetCount(sys::Service *serv);
    static bool SMSTemplateGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit);
    static auto GetQuery(sys::Service *serv, db::Interface::Name database, std::unique_ptr<db::Query> query) -> bool;
    static sys::SendResult GetQueryWithReply(sys::Service *serv,
                                             db::Interface::Name database,
                                             std::unique_ptr<db::Query> query,
                                             std::uint32_t timeout);

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
    static ContactVerificationError verifyContact(sys::Service *serv, const ContactRecord &rec);
    static std::string getVerificationErrorString(const ContactVerificationError err);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetByName(sys::Service *serv,
                                                                        UTF8 primaryName,
                                                                        UTF8 alternativeName);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetByID(sys::Service *serv, uint32_t contactID);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetByIDWithTemporary(sys::Service *serv,
                                                                                   uint32_t contactID);

  private:
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetByIDCommon(
        sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg);

  public:
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetBySpeeddial(sys::Service *serv, UTF8 speeddial);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetByPhoneNumber(sys::Service *serv, UTF8 phoneNumber);

    /**
     * @brief Matches single Contact by a provided phone number
     *
     * @param serv - calling service
     * @param numberView - number to match contact with
     * @return std::unique_ptr<ContactRecord>
     */
    static std::unique_ptr<ContactRecord> MatchContactByPhoneNumber(sys::Service *serv,
                                                                    const utils::PhoneNumber::View &numberView);
    static bool ContactAdd(sys::Service *serv, const ContactRecord &rec);
    static bool ContactRemove(sys::Service *serv, uint32_t id);
    static bool ContactUpdate(sys::Service *serv, const ContactRecord &rec);
    static bool ContactBlock(sys::Service *serv, uint32_t id, const bool shouldBeBlocked = true);
    static uint32_t ContactGetCount(sys::Service *serv, bool favourites = false);
    static bool ContactGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit);
    static std::unique_ptr<std::vector<ContactRecord>> ContactSearch(sys::Service *serv,
                                                                     UTF8 primaryName,
                                                                     UTF8 alternativeName,
                                                                     UTF8 number);
    static std::unique_ptr<utils::PhoneNumber::View> GetNumberById(sys::Service *serv,
                                                                   std::uint32_t numberId,
                                                                   std::uint32_t timeout = DefaultTimeoutInMs);

    static bool NotesAdd(sys::Service *serv, const NotesRecord &rec);
    static bool NotesRemove(sys::Service *serv, uint32_t id);
    static bool NotesUpdate(sys::Service *serv, const NotesRecord &rec);
    static uint32_t NotesGetCount(sys::Service *serv);
    static bool NotesGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit);

    static CalllogRecord CalllogAdd(sys::Service *serv, const CalllogRecord &rec);
    static bool CalllogRemove(sys::Service *serv, uint32_t id);
    static bool CalllogUpdate(sys::Service *serv, const CalllogRecord &rec);
    static uint32_t CalllogGetCount(sys::Service *serv, EntryState state = EntryState::ALL);
    static bool CalllogGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit);

    /* country codes */
    static uint32_t GetCountryCodeByMCC(sys::Service *serv, uint32_t mcc);

    static bool DBBackup(sys::Service *serv, std::string backupPath);
};
