// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OperatorInfo.hpp"
#include <string>
#include <optional>

namespace cellular::service
{
    class ImsiParserInteface
    {
      public:
        virtual ~ImsiParserInteface() = default;

        virtual auto parse(const std::string &imsi) -> std::optional<OperatorInfo> = 0;
    };
} // namespace cellular::service
