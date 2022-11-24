// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <string>

namespace at
{
    class Result;
    class BaseChannel;
} // namespace at

namespace cellular::service
{

    class VolteCapabilityCellularInterface
    {
      public:
        virtual ~VolteCapabilityCellularInterface()          = default;
        virtual void setChannel(at::BaseChannel *channel)    = 0;
        virtual auto getImsi() -> std::optional<std::string> = 0;
    };
} // namespace cellular::service
