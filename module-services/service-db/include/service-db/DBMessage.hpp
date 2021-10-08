// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Common.hpp>
#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <memory>

class DBMessage : public sys::DataMessage
{
  public:
    DBMessage(MessageType messageType) : sys::DataMessage(messageType){};
    virtual ~DBMessage(){};

    uint32_t id     = 0;
    uint32_t offset = 0;
    uint32_t limit  = 0;
};

class DBResponseMessage : public sys::ResponseMessage
{
  public:
    DBResponseMessage(uint32_t retCode, uint32_t count, MessageType responseTo)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode), count(count){};
    virtual ~DBResponseMessage(){};

    uint32_t retCode;
    uint32_t count;
};
