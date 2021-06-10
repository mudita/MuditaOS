// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControlParameters.hpp"

#include <gsl/assert>

namespace screen_light_control
{
    auto Parameters::getAutoModeParams() const noexcept -> const AutomaticModeParameters &
    {
        Expects(hasAutoModeParams());
        return autoModeParams.value();
    }

    auto Parameters::getManualModeParams() const noexcept -> ManualModeParameters
    {
        Expects(hasManualModeParams());
        return manualModeParams.value();
    }
} // namespace screen_light_control
