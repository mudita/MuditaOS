// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <popups/lock-popups/phone-lock-wallpapers/WallpaperBase.hpp>

namespace gui
{
    class WallpaperPresenter : public app::AsyncCallbackReceiver
    {
      public:
        WallpaperPresenter(app::ApplicationCommon *app);
        void attachWallpapers(std::shared_ptr<WallpaperBase> clockWallpaper,
                              std::shared_ptr<WallpaperBase> quoteWallpaper,
                              std::shared_ptr<WallpaperBase> logoWallpaper);
        void setupWallpaper();

      private:
        void buildWallpapers();
        app::ApplicationCommon *application;
        std::shared_ptr<WallpaperBase> clockWallpaper = nullptr;
        std::shared_ptr<WallpaperBase> quoteWallpaper = nullptr;
        std::shared_ptr<WallpaperBase> logoWallpaper  = nullptr;
    };

} // namespace gui
