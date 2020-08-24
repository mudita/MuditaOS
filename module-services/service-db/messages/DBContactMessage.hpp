#pragma once

#include "MessageType.hpp"

#include "DBMessage.hpp"
#include "Interface/ContactRecord.hpp"
#include "Service/Message.hpp"

#include <PhoneNumber.hpp>

#include <memory>
#include <utility>

class DBContactMessage : public DBMessage
{
  public:
    DBContactMessage(MessageType messageType, const ContactRecord &rec = ContactRecord{}, bool favourite = false);

    ContactRecord record;
    bool favourite = false;
    bool withTemporary = false;
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

class DBContactBlock : public DBMessage
{
  public:
    DBContactBlock(MessageType messageType, const uint32_t _id, bool _shouldBeBlocked);

    uint32_t id;
    bool shouldBeBlocked;
};

class DBContactSearchMessage : public DBMessage
{
  public:
    DBContactSearchMessage(MessageType messageType,
                           const char *_primaryName,
                           const char *_alternativeName,
                           const char *_number);

    const char *primaryName;
    const char *alternativeName;
    const char *number;
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
