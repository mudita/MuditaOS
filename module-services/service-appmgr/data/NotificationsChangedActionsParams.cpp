// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsChangedActionsParams.hpp"

#include <apps-common/notifications/NotificationData.hpp>

using namespace app::manager::actions;

NotificationsChangedParams::NotificationsChangedParams(Notifications notifications, bool showNotificationsWhenLocked)
    : notifications{std::move(notifications)}, showWhenLocked{showNotificationsWhenLocked}
{}

auto NotificationsChangedParams::getNotifications() const noexcept -> const Notifications &
{
    return notifications;
}

auto NotificationsChangedParams::showNotificationsWhenLocked() const noexcept -> bool
{
    return showWhenLocked;
}
