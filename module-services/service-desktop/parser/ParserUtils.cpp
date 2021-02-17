// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ParserUtils.hpp"

#include <log/log.hpp>

namespace parserFSM
{

    bool http::isMethodValid(uint8_t method)
    {
        if (method == static_cast<uint8_t>(http::Method::get) || method == static_cast<uint8_t>(http::Method::post) ||
            method == static_cast<uint8_t>(http::Method::put) || method == static_cast<uint8_t>(http::Method::del)) {
            return true;
        }
        else {
            LOG_ERROR("Invalid method!");
            return false;
        }
    }
} // namespace parserFSM
