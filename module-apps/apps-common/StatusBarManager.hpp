// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <StatusBar.hpp>

namespace app
{
    class StatusBarManager
    {
      public:
        void enableIndicators(const gui::status_bar::Indicators &indicators);
        [[nodiscard]] auto getConfiguration() const noexcept -> const gui::status_bar::Configuration &;
        void set(gui::status_bar::Indicator indicator, std::shared_ptr<StatusBarVisitor> config);

      private:
        gui::status_bar::Configuration statusBarConfiguration;
    };
} // namespace app
