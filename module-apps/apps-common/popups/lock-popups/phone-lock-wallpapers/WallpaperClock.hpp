// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"
#include <widgets/ClockDateWidget.hpp>
#include <notifications/NotificationsListPresenter.hpp>
#include <ListView.hpp>

namespace gui
{
    class WallpaperClock : public WallpaperBase
    {
      public:
        gui::ClockDateWidget *clockDate                             = nullptr;
        gui::ListView *notificationsList                            = nullptr;
        std::shared_ptr<gui::NotificationsListPresenter> notificationsListPresenter;

      public:
        WallpaperClock(gui::Item *parent);
        void show() override;
        void hide() override;
        std::shared_ptr<NotificationsPresenter> getNotificationsPresenter() override;

        void updateTime();
    };

} /* namespace gui */
