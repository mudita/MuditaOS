// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperPresenter.hpp"
#include <service-appmgr/messages/GetWallpaperOptionRequest.hpp>
#include <service-appmgr/Constants.hpp>

namespace gui
{
    WallpaperPresenter::WallpaperPresenter(app::ApplicationCommon *app) : AsyncCallbackReceiver(app), application(app)
    {}

    void WallpaperPresenter::attachWallpapers(std::shared_ptr<WallpaperBase> clockWallpaper,
                                              std::shared_ptr<WallpaperBase> quoteWallpaper,
                                              std::shared_ptr<WallpaperBase> logoWallpaper)
    {
        this->clockWallpaper = clockWallpaper;
        this->quoteWallpaper = quoteWallpaper;
        this->logoWallpaper  = logoWallpaper;
    }

    void WallpaperPresenter::setupWallpaper()
    {
        buildWallpapers();

        auto request = std::make_unique<app::manager::GetWallpaperOptionRequest>();
        auto task    = app::AsyncRequest::createFromMessage(std::move(request), service::name::appmgr);
        auto cb      = [&](auto response) {
            auto result = dynamic_cast<app::manager::GetWallpaperOptionResponse *>(response);
            switch (result->getWallpaperOption()) {
            case WallpaperOption::Clock:
                clockWallpaper->show();
                quoteWallpaper->hide();
                logoWallpaper->hide();
                break;
            case WallpaperOption::Quote:
                clockWallpaper->hide();
                quoteWallpaper->show();
                logoWallpaper->hide();
                break;
            case WallpaperOption::Logo:
                clockWallpaper->hide();
                quoteWallpaper->hide();
                logoWallpaper->show();
                break;
            }
            return true;
        };
        task->execute(application, this, cb);
    }

    void WallpaperPresenter::buildWallpapers()
    {
        clockWallpaper->build();
        quoteWallpaper->build();
        logoWallpaper->build();
    }

} // namespace gui
