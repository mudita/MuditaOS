// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
