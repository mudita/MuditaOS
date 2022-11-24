// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParserInterface.hpp"

namespace cellular::service
{
    class ImsiParserUS : public ImsiParserInteface
    {
      public:
        auto parse(const std::string &imsi) -> std::optional<OperatorInfo> final;
    };
} // namespace cellular::service
