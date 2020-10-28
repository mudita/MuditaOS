// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>    // for uint32_t
#include <sys/types.h> // for time_t
#include <memory>      // for unique_ptr
#include <vector>      // for vector

#include "DBMessage.hpp"              // for DBMessage, DBResponseMessage
#include "Interface/AlarmsRecord.hpp" // for AlarmsRecord
#include "MessageType.hpp"            // for MessageType, MessageType::MessageTypeUninitialized

class DBAlarmMessage : public DBMessage
{
  public:
    DBAlarmMessage(MessageType messageType, const AlarmsRecord &rec = AlarmsRecord{});
    AlarmsRecord record;
    time_t time = 0;
};

class DBAlarmResponseMessage : public DBResponseMessage
{
  public:
    DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,
                           uint32_t retCode   = 0,
                           uint32_t count     = 0,
                           MessageType respTo = MessageType::MessageTypeUninitialized);
    std::unique_ptr<std::vector<AlarmsRecord>> records;
};
