// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/data/WallpaperOption.hpp>

namespace wallpaper
{
    class WallpaperDisplayModel
    {
      private:
        gui::WallpaperOption wallpaperOption = gui::WallpaperOption::Clock;

      public:
        WallpaperDisplayModel() = default;

        void setWallpaper(gui::WallpaperOption option);
        [[nodiscard]] gui::WallpaperOption getWallpaper();
    };

} // namespace wallpaper
