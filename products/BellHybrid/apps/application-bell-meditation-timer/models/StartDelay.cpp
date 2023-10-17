// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StartDelay.hpp"
#include <Utils.hpp>

namespace
{
    constexpr auto start_delay_db = "start_delay";
} // namespace

namespace app::meditation::models
{

    void StartDelay::setValue(std::uint8_t value)
    {
        settings.setValue(start_delay_db, std::to_string(value));
    }
    std::uint8_t StartDelay::getValue() const
    {
        constexpr auto defaultValue = 30U;
        const auto value            = settings.getValue(start_delay_db);
        if (value.empty()) {
            return defaultValue;
        }
        return utils::getNumericValue<std::uint32_t>(value);
    }
} // namespace app::meditation::models
