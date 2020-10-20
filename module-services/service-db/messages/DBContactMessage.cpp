// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBContactMessage.hpp"
#include "MessageType.hpp"

#include <memory>
#include <utility>

DBContactMessage ::DBContactMessage(MessageType messageType, const ContactRecord &rec, bool favourite)
    : DBMessage(messageType), record(rec), favourite{favourite}
{}

DBContactBlock::DBContactBlock(MessageType messageType, const uint32_t _id, bool _shouldBeBlocked)
    : DBMessage(messageType), id(_id), shouldBeBlocked(_shouldBeBlocked)
{}

DBContactSearchMessage ::DBContactSearchMessage(MessageType messageType,
                                                const char *_primaryName,
                                                const char *_alternativeName,
                                                const char *_number)
    : DBMessage(messageType), primaryName(_primaryName), alternativeName(_alternativeName), number(_number)
{}

DBContactResponseMessage ::DBContactResponseMessage(std::unique_ptr<std::vector<ContactRecord>> rec,
                                                    uint32_t retCode,
                                                    uint32_t limit,
                                                    uint32_t offset,
                                                    bool favourite,
                                                    uint32_t count,
                                                    MessageType respTo)
    : DBResponseMessage(retCode, count, respTo),
      records(std::move(rec)), favourite{favourite}, limit{limit}, offset{offset}
{}

DBContactNumberMessage::DBContactNumberMessage(const utils::PhoneNumber::View &numberView)
    : sys::DataMessage(MessageType::DBContactMatchByNumber), numberView(numberView)
{}

DBContactNumberMessage::~DBContactNumberMessage()
{}

DBContactNumberResponseMessage::DBContactNumberResponseMessage(sys::ReturnCodes retCode,
                                                               std::unique_ptr<ContactRecord> contact)
    : sys::ResponseMessage(retCode, MessageType::DBContactMatchByNumber), contact(std::move(contact))
{}

DBContactNumberResponseMessage::~DBContactNumberResponseMessage()
{}
