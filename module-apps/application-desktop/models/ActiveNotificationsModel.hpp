// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <notifications/NotificationsModel.hpp>

namespace gui
{
    class ActiveNotificationsModel : public gui::NotificationsModel, public app::AsyncCallbackReceiver
    {
      private:
        AppWindow *parent = nullptr;
      public:
        explicit ActiveNotificationsModel(AppWindow *parent);
        void setParentNavBar(const UTF8 &left, const UTF8 &center, const UTF8 &right);
        auto create(const notifications::NotSeenSMSNotification *notification) -> NotificationListItem * override;
        auto create(const notifications::NotSeenCallNotification *notification) -> NotificationListItem * override;
        auto create(const notifications::TetheringNotification *notification) -> NotificationListItem * override;
        auto create(const notifications::AlarmSnoozeNotification *notification) -> NotificationListItem * override;
    };
} // namespace gui
