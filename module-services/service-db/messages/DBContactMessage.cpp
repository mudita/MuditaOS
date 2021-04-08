// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBContactMessage.hpp>

#include <ContactRecord.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>

#include <memory>
#include <utility>

DBContactMessage ::DBContactMessage(MessageType messageType, const ContactRecord &rec, bool favourite)
    : DBMessage(messageType), record(rec), favourite{favourite}
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
