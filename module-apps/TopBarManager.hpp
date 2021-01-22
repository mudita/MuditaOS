// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <TopBar.hpp>

namespace app
{
    class TopBarManager
    {
      public:
        void enableIndicators(const gui::top_bar::Indicators &indicators);
        [[nodiscard]] auto getConfiguration() const noexcept -> const gui::top_bar::Configuration &;

      private:
        gui::top_bar::Configuration topBarConfiguration;
    };
} // namespace app
