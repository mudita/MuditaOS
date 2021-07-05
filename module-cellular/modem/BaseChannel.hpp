// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>
#include <at/Commands.hpp>
#include <at/Result.hpp>
#include <bsp/cellular/CellularResult.hpp>

namespace at
{
    struct Cmd;

    class BaseChannel
    {
      public:
        virtual ~BaseChannel() = default;
        /// @defgroup Channel OS dependent methods
        /// {

        /// waits till ok or timeout
        virtual auto cmd(const std::string &cmd,
                         std::chrono::milliseconds timeout = at::default_timeout,
                         size_t rxCount                    = 0) -> Result = 0;
        virtual auto cmd(const at::AT &at) -> Result   = 0;
        virtual auto cmd(const at::Cmd &at) -> Result  = 0;
        /// }

        /// @defgroup Channel Platform independent methods
        /// {
        virtual void cmdLog(std::string cmd, const Result &result, std::chrono::milliseconds timeout)
        {}
        virtual void cmdInit()                                                               = 0;
        virtual void cmdSend(std::string cmd)                                                = 0;
        virtual size_t cmdReceive(std::uint8_t *buffer, std::chrono::milliseconds timeoutMs) = 0;
        virtual void cmdPost()                                                               = 0;
        /// }
    };
} // namespace at
