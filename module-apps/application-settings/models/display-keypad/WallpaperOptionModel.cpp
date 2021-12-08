// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperOptionModel.hpp"

namespace gui
{
    WallpaperOptionModel::WallpaperOptionModel(app::settingsInterface::WallpaperSettings *settings) : settings(settings)
    {}

    void WallpaperOptionModel::update()
    {
        wallpaperOption = settings->getWallpaperOption();
    }

    void WallpaperOptionModel::saveValue(WallpaperOption option)
    {
        settings->setWallpaperOption(option);
        wallpaperOption = option;
    }

    auto WallpaperOptionModel::isCurrentOption(WallpaperOption option) -> bool
    {
        return wallpaperOption == option;
    }

    auto WallpaperOptionModel::isQuoteOptionSelected() -> bool
    {
        return wallpaperOption == WallpaperOption::Quote;
    }
} // namespace gui
