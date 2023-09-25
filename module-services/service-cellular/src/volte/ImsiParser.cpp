// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImsiParser.hpp"

namespace cellular::service
{
    auto ImsiParser::isAllowed(const std::string &imsi) const -> bool
    {
        if (operatorCodes.empty()) {
            return false;
        }

        for (auto const &code : operatorCodes) {
            if (textStartsWith(imsi, code)) {
                return true;
            }
        }

        return false;
    }

    auto ImsiParser::getSupportStatus() const -> SupportStatus
    {
        return supportStatus;
    }

    auto ImsiParser::textStartsWith(std::string_view text, std::string_view prefix) const -> bool
    {
        return !text.empty() && !prefix.empty() && std::equal(prefix.begin(), prefix.end(), text.begin());
    }
} // namespace cellular
