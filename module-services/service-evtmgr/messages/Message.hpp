// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>

namespace sevm
{
    struct Message : public sys::DataMessage
    {
        Message(MessageType messageType);
    };
}; // namespace sevm
