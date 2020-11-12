// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBAlarmMessage.hpp>

#include <AlarmsRecord.hpp>
#include <service-db/DBMessage.hpp>

#include <utility> // for move

DBAlarmMessage ::DBAlarmMessage(MessageType messageType, const AlarmsRecord &rec) : DBMessage(messageType), record(rec)
{}

DBAlarmResponseMessage::DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,
                                               uint32_t retCode,
                                               uint32_t count,
                                               MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec))
{}
