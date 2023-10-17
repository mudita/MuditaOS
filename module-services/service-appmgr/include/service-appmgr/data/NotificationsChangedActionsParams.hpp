// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

#include <list>

namespace notifications
{
    class Notification;
}

namespace app::manager::actions
{
    class NotificationsChangedParams : public app::manager::actions::ActionParams
    {
      public:
        using Notifications = std::list<std::shared_ptr<const notifications::Notification>>;
        explicit NotificationsChangedParams(Notifications notifications,
                                            bool showWhenLocked,
                                            bool fastRefreshOnUpdate = false);

        [[nodiscard]] auto getNotifications() const noexcept -> const Notifications &;
        [[nodiscard]] auto showNotificationsWhenLocked() const noexcept -> bool;
        [[nodiscard]] auto fastRefreshOnNotificationUpdate() const noexcept -> bool;

      private:
        Notifications notifications;
        const bool showWhenLocked;
        const bool fastRefreshOnUpdate;
    };
} // namespace app::manager::actions
