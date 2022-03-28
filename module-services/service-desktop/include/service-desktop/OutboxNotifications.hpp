// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DBNotificationMessage.hpp>
#include <service-desktop/Outbox.hpp>

class OutboxNotifications
{
  public:
    void clearNotifications();
    void newNotificationHandler(db::NotificationMessage *notificationMessage);
    auto getNotificationEntries() const -> std::vector<Outbox::NotificationEntry>;
    void removeNotificationEntries(const std::vector<uint32_t> &);

  private:
    uint32_t notificationCurrentUid = 0;
    std::vector<Outbox::NotificationEntry> notificationEntries;
};
