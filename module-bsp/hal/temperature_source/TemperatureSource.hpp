// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>

namespace bsp::hal
{
    class TemperatureSource
    {
      public:
        using Temperature     = float; ///< Celsius degrees
        using Result          = std::optional<Temperature>;
        virtual Result read() = 0;
    };
} // namespace bsp::hal
