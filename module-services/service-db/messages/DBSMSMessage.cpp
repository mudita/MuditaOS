// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBMessage.hpp>
#include <service-db/DBSMSMessage.hpp>

#include <SMSRecord.hpp>

#include <utility> // for move

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
