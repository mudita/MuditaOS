// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// Please do not pollute here
// This is straight forward declaration header for includes purposes

#include <memory>
#include <system/SystemReturnCodes.hpp>

namespace sys
{
    class Service;
    class Message;
    using MessagePointer = std::shared_ptr<Message>;
    using MessageNone    = std::nullptr_t;
    using SendResult     = std::pair<ReturnCodes, std::shared_ptr<Message>>;
} // namespace sys
