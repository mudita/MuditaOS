// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>
#include <at/Commands.hpp>
#include <at/Result.hpp>

namespace at
{
    class BaseChannel
    {
      public:
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
        virtual void cmd_log(std::string cmd, const Result &result, uint32_t timeout)
        {}
        virtual void cmd_init()                = 0;
        virtual void cmd_send(std::string cmd) = 0;
        virtual std::string cmd_receive()      = 0;
        virtual void cmd_post()                = 0;
        /// }
    };
} // namespace at
