// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace sevm
{
    struct Message : public sys::DataMessage
    {
        Message(MessageType messageType);
        Message();
    };
}; // namespace sevm
