// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"

#include <Interface/ContactRecord.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <system/Common.hpp>
#include <Service/Message.hpp>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

class DBContactMessage : public DBMessage
{
  public:
    DBContactMessage(MessageType messageType, const ContactRecord &rec = ContactRecord{}, bool favourite = false);

    ContactRecord record;
    bool favourite     = false;
    bool withTemporary = false;
};

class DBMatchContactByNumberIDMessage : public sys::DataMessage
{
  public:
    explicit DBMatchContactByNumberIDMessage(std::uint32_t numberID);

    const std::uint32_t numberID;
};

/**
 * @brief Message used to match Contact by an instance of PhoneNumber::View
 *
 */
class DBContactNumberMessage : public sys::DataMessage
{
  public:
    /**
     * @brief Construct a new DBContactNumberMessage object
     *
     * @param numberView - a number to match with
     */
    DBContactNumberMessage(const utils::PhoneNumber::View &numberView);

    /**
     * @brief Destroy the DBContactNumberMessage object
     *
     */
    virtual ~DBContactNumberMessage();

    /**
     * @brief number to match with
     *
     */
    utils::PhoneNumber::View numberView;
};

/**
 * @brief Message used to match Contact by an instance of PhoneNumber::View while omitting the specific contactID.
 *        Omitting specific Contact during searching is helpful to ignore the number which is already signed to Contact
 *        that we want to find a matching number for
 *
 */
class DBMatchContactNumberBesidesOfContactIDMessage : public sys::DataMessage
{
  public:
    /**
     * @brief Construct a new DBMatchContactNumberBesidesOfContactIDMessage object
     *
     * @param numberView - a number to match with
     */

    DBMatchContactNumberBesidesOfContactIDMessage(const utils::PhoneNumber::View &numberView,
                                                  std::uint32_t contactIDToOmit);
    /**
     * @brief Destroy the DBMatchContactNumberBesidesOfContactIDMessage object
     *
     */
    virtual ~DBMatchContactNumberBesidesOfContactIDMessage();

    /**
     * @brief number to match with
     *
     */
    utils::PhoneNumber::View numberView;

    /**
     * @brief numbers connected with this contact Id are omitting during the searching
     *
     */
    std::uint32_t contactIDToOmit;
};

/**
 * @brief A response to DBContactNumberMessage - returns an instance of a ContactRecord matched by provided
 * PhoneNumber::View
 *
 */
class DBContactNumberResponseMessage : public sys::ResponseMessage
{
  public:
    /**
     * @brief Construct a new DBContactNumberResponseMessage object
     *
     * @param retCode - operation result
     * @param contact - matched contact (could be an empty unique_ptr)
     */
    DBContactNumberResponseMessage(sys::ReturnCodes retCode, std::unique_ptr<ContactRecord> contact);

    /**
     * @brief Destroy the DBContactNumberResponseMessage object
     *
     */
    virtual ~DBContactNumberResponseMessage();

    /**
     * @brief Matched contact
     *
     */
    std::unique_ptr<ContactRecord> contact;
};

class DBContactResponseMessage : public DBResponseMessage
{
  public:
    DBContactResponseMessage(std::unique_ptr<std::vector<ContactRecord>> rec,
                             uint32_t retCode   = 0,
                             uint32_t limit     = 0,
                             uint32_t offset    = 0,
                             bool favourite     = false,
                             uint32_t count     = 0,
                             MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<ContactRecord>> records;
    bool favourite  = false;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
