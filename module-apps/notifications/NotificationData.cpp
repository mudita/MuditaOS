// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationData.hpp"

uint32_t notifications::Notification::priorityPool = 0;

using namespace notifications;

Notification::Notification(NotificationType type, NotificationPriority priorityType) : type{type}
{
    switch (priorityType) {
    case NotificationPriority::Next:
        if (priorityPool == highestPriority) {
            priorityPool = 0;
        }
        priority = ++priorityPool;
        break;
    case NotificationPriority::Highest:
        priority = highestPriority;
        break;
    case NotificationPriority::Lowest:
        priority = lowestPriority;
        break;
    }
}

auto Notification::getType() const noexcept -> NotificationType
{
    return type;
}

auto Notification::getPriority() const noexcept -> uint32_t
{
    return priority;
}

NotSeenSMSNotification::NotSeenSMSNotification(unsigned value)
    : Notification(NotificationType::NotSeenSms), value{value}
{}

auto NotSeenSMSNotification::getValue() const noexcept -> unsigned
{
    return value;
}

NotSeenCallNotification::NotSeenCallNotification(unsigned value, std::unique_ptr<ContactRecord> record)
    : Notification(NotificationType::NotSeenCall), value{value}, record{std::move(record)}
{}

bool NotSeenCallNotification::hasRecord() const noexcept
{
    return record != nullptr;
}

auto NotSeenCallNotification::getRecord() const noexcept -> const std::unique_ptr<ContactRecord> &
{
    return record;
}

auto NotSeenCallNotification::getValue() const noexcept -> unsigned
{
    return value;
}
