// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/ApplicationSettings.hpp>

namespace gui
{
    class WallpaperOptionModel
    {
      public:
        explicit WallpaperOptionModel(app::settingsInterface::WallpaperSettings *settings);
        void update();
        void saveValue(WallpaperOption option);
        auto isCurrentOption(WallpaperOption option) -> bool;
        auto isQuoteOptionSelected() -> bool;

      private:
        app::settingsInterface::WallpaperSettings *settings = nullptr;
        WallpaperOption wallpaperOption                     = WallpaperOption::Clock;
    };
} // namespace gui
