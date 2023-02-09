// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsData.hpp"
#include "Constants.hpp"

namespace gui
{
    OptionsData::OptionsData(OptionParams params) : params(params)
    {}

    [[nodiscard]] std::chrono::seconds OptionsData::getPreparationTime() const noexcept
    {
        return params.preparationTime.get();
    }

    bool OptionsData::setPreparationTime(std::chrono::seconds value)
    {
        bool result   = true;
        const auto it = std::find(
            std::begin(Constants::Params::preparationTimes), std::end(Constants::Params::preparationTimes), value);
        if (it == std::end(Constants::Params::preparationTimes)) {
            value  = Constants::Params::defaultPreparationTime;
            result = false;
        }
        params.preparationTime.set(value);
        return result;
    }

    [[nodiscard]] bool OptionsData::isCounterVisible() const noexcept
    {
        return params.showCounter.get();
    }

    void OptionsData::setCounterVisible(bool value)
    {
        params.showCounter.set(value);
    }
} // namespace gui
