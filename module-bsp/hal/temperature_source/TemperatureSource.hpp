// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <optional>

namespace hal::temperature
{
    class AbstractTemperatureSource
    {
      public:
        struct Factory
        {
            static std::shared_ptr<AbstractTemperatureSource> create();
        };

        virtual ~AbstractTemperatureSource() = default;

        using Temperature = float; ///< Celsius degrees
        using Result      = std::optional<Temperature>;

        virtual Result read() = 0;
    };
} // namespace hal::temperature
