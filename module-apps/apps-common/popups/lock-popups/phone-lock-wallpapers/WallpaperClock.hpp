// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"
#include <widgets/ClockDateWidget.hpp>
#include <notifications/NotificationsModel.hpp>
#include <ListView.hpp>

namespace gui
{
    class WallpaperClock : public WallpaperBase
    {
      private:
        gui::ClockDateWidget *clockDate                             = nullptr;
        gui::ListView *notificationsList                            = nullptr;
        std::shared_ptr<gui::NotificationsModel> notificationsModel = nullptr;

      public:
        WallpaperClock(gui::Item *parent, std::shared_ptr<gui::NotificationsModel> notificationsModel);
        void build() override;
        void show() override;
        void hide() override;

        void updateTime();
    };

} /* namespace gui */
