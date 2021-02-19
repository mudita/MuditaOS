// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TopBarManager.hpp"
#include "TopBar.hpp"

namespace app
{
    void TopBarManager::enableIndicators(const gui::top_bar::Indicators &indicators)
    {
        topBarConfiguration.enable(indicators);
    }

    auto TopBarManager::getConfiguration() const noexcept -> const gui::top_bar::Configuration &
    {
        return topBarConfiguration;
    }

    void TopBarManager::set(gui::top_bar::TimeMode timeMode)
    {
        topBarConfiguration.set(timeMode);
    }
} // namespace app
