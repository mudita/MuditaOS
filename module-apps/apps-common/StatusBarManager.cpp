// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatusBarManager.hpp"
#include "StatusBar.hpp"

namespace app
{
    void StatusBarManager::enableIndicators(const gui::status_bar::Indicators &indicators)
    {
        statusBarConfiguration.enable(indicators);
    }

    auto StatusBarManager::getConfiguration() const noexcept -> const gui::status_bar::Configuration &
    {
        return statusBarConfiguration;
    }

    void StatusBarManager::set(gui::status_bar::Indicator indicator, std::shared_ptr<StatusBarVisitor> config)
    {
        statusBarConfiguration.setIndicatorModifier(indicator, std::move(config));
    }
} // namespace app
