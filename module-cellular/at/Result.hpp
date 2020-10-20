// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

namespace at
{
    struct Result
    {
        /// result class for AT send -> receive command, could return promise :p
        enum class Code
        {
            OK,      // at OK
            ERROR,   // at ERROR
            TIMEOUT, // at Timeout
            TOKENS,  // at numbers of tokens needed met
            NONE,    // no code
        } code = Code::NONE;
        std::vector<std::string> response;
        explicit operator bool() const
        {
            return code == Code::OK;
        }
    };
} // namespace at
