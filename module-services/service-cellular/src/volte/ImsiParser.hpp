// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace cellular::service
{
    class ImsiParser
    {
      public:
        explicit ImsiParser(std::vector<std::string> &&operatorList) : operatorCodes{operatorList}
        {}
        auto isAllowed(const std::string &imsi) const -> bool;

      private:
        std::vector<std::string> operatorCodes;
        inline auto textStartsWith(std::string_view text, std::string_view prefix) const -> bool;
    };
} // namespace cellular::service
