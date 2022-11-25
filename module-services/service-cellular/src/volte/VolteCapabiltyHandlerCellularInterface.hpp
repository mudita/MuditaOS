// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <string>

namespace at
{
    class BaseChannel;
}

namespace cellular::service
{
    class VolteCapabilityCellularInterface
    {
      public:
        virtual ~VolteCapabilityCellularInterface()          = default;
        virtual auto getImsi(at::BaseChannel &) -> std::optional<std::string> = 0;
    };
} // namespace cellular::service
