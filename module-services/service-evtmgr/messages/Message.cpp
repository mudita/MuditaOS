// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Message.hpp"

#include "MessageType.hpp"     // for MessageType
#include "Service/Message.hpp" // for DataMessage

namespace sevm
{
    Message::Message(MessageType messageType) : DataMessage(messageType)
    {}
} // namespace sevm
