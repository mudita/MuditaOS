// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "MessageForward.hpp"
#include <functional>

namespace sys
{
    using MessageHandler = std::function<MessagePointer(Message *)>;

    struct Proxy;
    class Timer;

    namespace timer
    {
        class SystemTimer;
    } // namespace timer
} // namespace sys
