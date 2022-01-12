// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationTilesPresenter.hpp"
#include <Image.hpp>
#include <Style.hpp>

using namespace gui;

namespace
{
    constexpr auto messagesIcon  = "messages_notification_icon_W_G";
    constexpr auto callsIcon     = "calls_notification_icon_W_G";
    constexpr auto tetheringIcon = "tethering_notification_icon_W_G";
    constexpr auto alarmIcon     = "alarm_notification_icon_W_G";
    constexpr auto lockIcon      = "lock_notification_icon_W_G";

    auto buildNotificationIcon(const UTF8 &imageName) -> gui::Image *
    {
        auto icon = new gui::Image(imageName);
        icon->setMinimumWidth(::style::wallpaper::notificationTiles::tileIconHeight);
        icon->setMargins(gui::Margins(::style::wallpaper::notificationTiles::tileIconMarigin,
                                      0,
                                      ::style::wallpaper::notificationTiles::tileIconMarigin,
                                      0));
        return icon;
    }
} // namespace

NotificationTilesPresenter::NotificationTilesPresenter()
{}

void NotificationTilesPresenter::attach(HBox *tilesBox)
{
    this->tilesBox = tilesBox;
}

void NotificationTilesPresenter::updateData(app::manager::actions::NotificationsChangedParams *params,
                                            bool callAndSMSVisibility)
{
    if (tilesBox == nullptr) {
        LOG_ERROR("TilesBox not attached");
        return;
    }

    tilesBox->erase();

    for (const auto &notification : params->getNotifications()) {
        if (typeid(*notification) == typeid(notifications::NotSeenSMSNotification) && callAndSMSVisibility) {
            tilesBox->addWidget(buildNotificationIcon(messagesIcon));
        }
        else if (typeid(*notification) == typeid(notifications::NotSeenCallNotification) && callAndSMSVisibility) {
            tilesBox->addWidget(buildNotificationIcon(callsIcon));
        }
        else if (typeid(*notification) == typeid(notifications::TetheringNotification)) {
            tilesBox->addWidget(buildNotificationIcon(tetheringIcon));
        }
        else if (typeid(*notification) == typeid(notifications::PhoneLockNotification)) {
            tilesBox->addWidget(buildNotificationIcon(lockIcon));
        }
        else if (typeid(*notification) == typeid(notifications::AlarmSnoozeNotification)) {
            tilesBox->addWidget(buildNotificationIcon(alarmIcon));
        }
    }
    tilesBox->resizeItems();
}

bool NotificationTilesPresenter::hasDismissibleNotification() const noexcept
{
    return false;
}

void NotificationTilesPresenter::dismissAll()
{}

bool NotificationTilesPresenter::isEmpty() const noexcept
{
    return false;
}
