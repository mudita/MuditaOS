// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <appmgr/models/WallpaperDisplayModel.hpp>

namespace wallpaper
{
    void WallpaperDisplayModel::setWallpaper(const gui::WallpaperOption option)
    {
        wallpaperOption = option;
    }

    gui::WallpaperOption WallpaperDisplayModel::getWallpaper()
    {
        return wallpaperOption;
    }
} // namespace wallpaper
