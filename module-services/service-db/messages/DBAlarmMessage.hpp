// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"
#include "Interface/AlarmsRecord.hpp"

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
