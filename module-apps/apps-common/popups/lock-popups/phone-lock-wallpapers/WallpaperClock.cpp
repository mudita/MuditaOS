// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperClock.hpp"
#include <service-appmgr/Controller.hpp>

namespace gui
{
    WallpaperClock::WallpaperClock(Item *parent)
        : WallpaperBase(parent), notificationsListPresenter(std::make_shared<NotificationsListPresenter>())
    {
        clockDate = new ClockDateWidget(
            parent, 0, style::window::default_vertical_pos, style::window_width, clock_date_widget::h);

        notificationsList = new ListView(parent,
                                         style::notifications::model::x,
                                         style::notifications::model::y,
                                         style::notifications::model::w,
                                         style::notifications::model::h,
                                         notificationsListPresenter,
                                         listview::ScrollBarType::None);
        hide();
    }

    void WallpaperClock::show()
    {
        clockDate->setVisible(true);
        notificationsList->setVisible(true);
    }

    void WallpaperClock::hide()
    {
        clockDate->setVisible(false);
        notificationsList->setVisible(false);
    }

    std::shared_ptr<NotificationsPresenter> WallpaperClock::getNotificationsPresenter()
    {
        return notificationsListPresenter;
    }

    void WallpaperClock::updateTime()
    {
        clockDate->setTime(std::time(nullptr));
    }
} /* namespace gui */
