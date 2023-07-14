// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/HttpEnums.hpp>

#include <log/log.hpp>

namespace sdesktop::endpoints::http
{
    auto isMethodValid(std::uint8_t method) -> bool
    {
        if (method == static_cast<std::uint8_t>(http::Method::Get) ||
            method == static_cast<std::uint8_t>(http::Method::Post) ||
            method == static_cast<std::uint8_t>(http::Method::Put) ||
            method == static_cast<std::uint8_t>(http::Method::Del)) {
            return true;
        }

        LOG_ERROR("Invalid method!");
        return false;
    }
} // namespace sdesktop::endpoints::http
