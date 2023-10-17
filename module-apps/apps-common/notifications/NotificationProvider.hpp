// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationData.hpp"
#include "NotificationsConfiguration.hpp"
#include "policies/NotificationsListPolicy.hpp"
#include <PhoneModes/Common.hpp>

namespace sys
{
    class Service;
}

class NotificationsRecord;

namespace db
{
    class NotificationMessage;
    namespace query::notifications
    {
        class GetAllResult;
    }
} // namespace db

namespace notifications
{
    class NotificationProvider
    {
        template <NotificationType type, typename T>
        bool handleNotSeenWithCounter(NotificationsRecord &&record);

      public:
        NotificationProvider(sys::Service *ownerService, NotificationsConfiguration &notificationConfig);

        void handle(locks::PhoneLockTimeUpdate *msg);
        void handle(db::query::notifications::GetAllResult *msg);
        void handle(db::NotificationMessage *msg);
        void handle(sys::phone_modes::Tethering tethering);
        void handleSnooze(unsigned snoozeCount);
        void handleBatteryTooHot(bool tooHot, bool allowUpdate = true);
        void requestNotSeenNotifications();
        void send(NotificationOnReceiveUpdate updateOnReceive = NotificationOnReceiveUpdate::FullRebuild);

        using Notifications = std::map<NotificationType, std::shared_ptr<Notification>>;

      private:
        sys::Service *ownerService;
        NotificationsConfiguration &notificationConfig;
        NotificationsListPolicy listPolicy;
        Notifications notifications;
    };
} // namespace notifications
