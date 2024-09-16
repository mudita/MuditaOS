// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace cellular::service
{
    class ImsiParser
    {
      public:
        enum class SupportStatus
        {
            Normal,
            Beta,
            Unsupported
        };

        explicit ImsiParser(std::vector<std::string> &&operatorList, SupportStatus supportStatus)
            : operatorCodes{operatorList}, supportStatus{supportStatus}
        {}
        auto isAllowed(const std::string &imsi) const -> bool;
        auto getSupportStatus() const -> SupportStatus;

      private:
        std::vector<std::string> operatorCodes;
        SupportStatus supportStatus;

        inline auto textStartsWith(std::string_view text, std::string_view prefix) const -> bool;
    };
} // namespace cellular::service
