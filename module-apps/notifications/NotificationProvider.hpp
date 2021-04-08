// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationData.hpp"

namespace sys
{
    class Service;
}

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
        template <NotificationType type, typename T> bool handleNotSeenWithCounter(unsigned int value);

      public:
        explicit NotificationProvider(sys::Service *ownerService);

        void handle(db::query::notifications::GetAllResult *msg);
        void handle(db::NotificationMessage *msg);
        void requestNotSeenNotifications();
        void send();

        using Notifications = std::map<NotificationType, std::shared_ptr<Notification>>;

      private:
        sys::Service *ownerService;
        Notifications notifications;
    };

} // namespace notifications
