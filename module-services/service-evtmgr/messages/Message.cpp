// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-evtmgr/Message.hpp>

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace sevm
{
    Message::Message(MessageType messageType) : DataMessage(messageType)
    {}

    Message::Message() = default;
} // namespace sevm
