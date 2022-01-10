// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "phone-lock-wallpapers/WallpaperClock.hpp"
#include "phone-lock-wallpapers/WallpaperQuote.hpp"
#include "phone-lock-wallpapers/WallpaperLogo.hpp"

#include <apps-common/popups/presenter/WallpaperPresenter.hpp>
#include <AppWindow.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PhoneLockedWindow : public AppWindow
    {
      public:
        PhoneLockedWindow(app::ApplicationCommon *app,
                          const std::string &name,
                          std::unique_ptr<WallpaperPresenter> &&presenter);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        bool updateTime() override;

      protected:
        gui::ClockDateWidget *clockDate                             = nullptr;
        gui::ListView *notificationsList                            = nullptr;
        std::shared_ptr<gui::NotificationsModel> notificationsModel = nullptr;
        bool refreshedOnPhoneLockTimeLock                           = false;

        bool processLongReleaseEvent(const InputEvent &inputEvent);

      private:
        static constexpr auto refreshTimerName = "PhoneLockRefreshTimer";
        static constexpr auto refreshTimeout   = std::chrono::hours(4);
        sys::TimerHandle screenRefreshTimer;

        std::shared_ptr<WallpaperClock> clockWallpaper;
        std::unique_ptr<WallpaperPresenter> wallpaperPresenter;
    };

} /* namespace gui */
