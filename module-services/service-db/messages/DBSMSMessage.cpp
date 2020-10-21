// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBSMSMessage.hpp"

#include "SMSRecord.hpp" // for SMSRecord

#include "messages/DBMessage.hpp" // for DBMessage, DBResponseMessage
#include <utility>                // for move

DBSMSMessage::DBSMSMessage(MessageType messageType, const SMSRecord &rec) : DBMessage(messageType), record(rec)
{}

DBSMSGetCount::DBSMSGetCount() : DBSMSMessage(MessageType::DBSMSGetCount)
{}

DBSMSResponseMessage::DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,
                                           uint32_t retCode,
                                           uint32_t count,
                                           MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec))
{}
