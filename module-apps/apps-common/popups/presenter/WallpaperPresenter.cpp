// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperPresenter.hpp"
#include <service-appmgr/Constants.hpp>

namespace gui
{
    WallpaperPresenter::WallpaperPresenter(app::ApplicationCommon *app)
        : AsyncCallbackReceiver(app), application(app),
          notificationsModel(std::make_shared<NotificationsModel>(NotificationsListPlacement::LockedScreen))
    {}

    void WallpaperPresenter::setupWallpaper(Item *parent)
    {
        buildWallpapers(parent);

        auto request = std::make_unique<app::manager::GetWallpaperOptionRequest>();
        auto task    = app::AsyncRequest::createFromMessage(std::move(request), service::name::appmgr);
        auto cb      = [&](auto response) {
            auto result = dynamic_cast<app::manager::GetWallpaperOptionResponse *>(response);
            if (!clockWallpaperForced) {
                switchWallpaper(result->getWallpaperOption());
            }
            selectedOption = result->getWallpaperOption();
            return true;
        };
        task->execute(application, this, cb);
    }

    void WallpaperPresenter::switchWallpaper(WallpaperOption option)
    {
        switch (option) {
        case WallpaperOption::Clock:
            notificationsModel->attachPresenter(clockWallpaper->getNotificationsPresenter());
            clockWallpaper->show();
            quoteWallpaper->hide();
            logoWallpaper->hide();
            break;
        case WallpaperOption::Quote:
            notificationsModel->attachPresenter(quoteWallpaper->getNotificationsPresenter());
            clockWallpaper->hide();
            quoteWallpaper->show();
            logoWallpaper->hide();
            break;
        case WallpaperOption::Logo:
            notificationsModel->attachPresenter(logoWallpaper->getNotificationsPresenter());
            clockWallpaper->hide();
            quoteWallpaper->hide();
            logoWallpaper->show();
            break;
        }
    }

    void WallpaperPresenter::buildWallpapers(Item *parent)
    {
        clockWallpaper = std::make_shared<WallpaperClock>(parent);
        quoteWallpaper = std::make_shared<WallpaperQuote>(application, parent);
        logoWallpaper  = std::make_shared<WallpaperLogo>(parent);
    }

    std::shared_ptr<gui::NotificationsModel> WallpaperPresenter::getNotificationsModel()
    {
        return notificationsModel;
    }

    bool WallpaperPresenter::updateWallpaper()
    {
        switch (selectedOption) {
        case WallpaperOption::Clock:
            if (clockWallpaper) {
                clockWallpaper->updateTime();
            }
            return true;
            break;
        default:
            return false;
            break;
        }
    }

    void WallpaperPresenter::forceClockWallpaper()
    {
        switchWallpaper(WallpaperOption::Clock);
        clockWallpaperForced = true;
    }

    bool WallpaperPresenter::switchBackWallpaper()
    {
        if (clockWallpaperForced) {
            clockWallpaperForced = false;
            switchWallpaper(selectedOption);
            return true;
        }
        else {
            return false;
        }
    }

} // namespace gui
