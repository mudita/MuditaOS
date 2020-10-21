// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBAlarmMessage.hpp"

#include "AlarmsRecord.hpp" // for AlarmsRecord

#include "messages/DBMessage.hpp" // for DBMessage, DBResponseMessage
#include <utility>                // for move

DBAlarmMessage ::DBAlarmMessage(MessageType messageType, const AlarmsRecord &rec) : DBMessage(messageType), record(rec)
{}

DBAlarmResponseMessage::DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,
                                               uint32_t retCode,
                                               uint32_t count,
                                               MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec))
{}
