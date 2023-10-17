// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
