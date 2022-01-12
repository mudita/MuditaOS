// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <service-appmgr/messages/GetWallpaperOptionRequest.hpp>
#include <popups/lock-popups/phone-lock-wallpapers/WallpaperClock.hpp>
#include <popups/lock-popups/phone-lock-wallpapers/WallpaperQuote.hpp>
#include <popups/lock-popups/phone-lock-wallpapers/WallpaperLogo.hpp>
#include <notifications/NotificationsModel.hpp>

namespace gui
{
    class WallpaperPresenter : public app::AsyncCallbackReceiver
    {
      public:
        WallpaperPresenter(app::ApplicationCommon *app);
        void setupWallpaper(Item *parent);
        std::shared_ptr<gui::NotificationsModel> getNotificationsModel();
        void updateTime();
        void forceClockWallpaper();
        /// returns true if actual switch back occured
        bool switchBackWallpaper();

      private:
        void buildWallpapers(Item *parent);
        void switchWallpaper(WallpaperOption option);
        app::ApplicationCommon *application;
        std::shared_ptr<gui::NotificationsModel> notificationsModel;
        std::shared_ptr<WallpaperClock> clockWallpaper = nullptr;
        std::shared_ptr<WallpaperQuote> quoteWallpaper = nullptr;
        std::shared_ptr<WallpaperLogo> logoWallpaper   = nullptr;
        bool clockWallpaperForced                      = false;
        WallpaperOption selectedOption                 = WallpaperOption::Clock;
    };

} // namespace gui
