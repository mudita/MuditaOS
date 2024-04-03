// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeepRefreshInterval.hpp"
#include <Utils.hpp>

namespace app::clock::models
{

    void DeepRefreshInterval::setValue(std::uint8_t value)
    {
        refreshInterval = value;
    }
    std::uint8_t DeepRefreshInterval::getValue() const
    {
        constexpr auto defaultValue = 15U;
        if (refreshInterval == 0) {
            return defaultValue;
        }
        return refreshInterval;
    }
} // namespace app::clock::models
