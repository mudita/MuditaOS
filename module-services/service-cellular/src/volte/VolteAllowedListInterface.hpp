// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace cellular::service
{
    class VolteAllowedListInterface
    {
      public:
        virtual ~VolteAllowedListInterface() = default;

        auto virtual isVolteAllowed(const std::string &imsi) const -> bool = 0;
    };
} // namespace cellular::service
