// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationsChangedActionsParams.hpp"

#include <apps-common/notifications/NotificationData.hpp>

using namespace app::manager::actions;

NotificationsChangedParams::NotificationsChangedParams(Notifications notifications,
                                                       bool showNotificationsWhenLocked,
                                                       bool fastRefreshOnUpdate)
    : notifications{std::move(notifications)}, showWhenLocked{showNotificationsWhenLocked}, fastRefreshOnUpdate{
                                                                                                fastRefreshOnUpdate}
{}

auto NotificationsChangedParams::getNotifications() const noexcept -> const Notifications &
{
    return notifications;
}

auto NotificationsChangedParams::showNotificationsWhenLocked() const noexcept -> bool
{
    return showWhenLocked;
}

auto NotificationsChangedParams::fastRefreshOnNotificationUpdate() const noexcept -> bool
{
    return fastRefreshOnUpdate;
}
