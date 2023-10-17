// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationData.hpp"
#include <gsl/assert>

namespace notifications
{
    std::uint32_t Notification::priorityPool = 0;

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

    auto Notification::getPriority() const noexcept -> std::uint32_t
    {
        return priority;
    }

    NotificationWithContact::NotificationWithContact(NotificationType type,
                                                     unsigned value,
                                                     std::optional<ContactRecord> record)
        : Notification(type), value{value}, record{std::move(record)}
    {}

    auto NotificationWithContact::hasRecord() const noexcept -> bool
    {
        return record.has_value();
    }

    auto NotificationWithContact::getRecord() const noexcept -> const ContactRecord &
    {
        Expects(hasRecord());
        return record.value();
    }

    auto NotificationWithContact::getValue() const noexcept -> unsigned
    {
        return value;
    }

    NotificationWithCounter::NotificationWithCounter(NotificationType type, unsigned value)
        : Notification(type), value{value}
    {}

    auto NotificationWithCounter::getValue() const noexcept -> unsigned
    {
        return value;
    }

    NotificationWithTime::NotificationWithTime(NotificationType type,
                                               NotificationPriority priorityType,
                                               std::string formattedTime)
        : Notification(type, priorityType), formattedTime(std::move(formattedTime))
    {}

    auto NotificationWithTime::getTime() const noexcept -> const std::string &
    {
        return formattedTime;
    }

    NotSeenSMSNotification::NotSeenSMSNotification(unsigned value, std::optional<ContactRecord> record)
        : NotificationWithContact(NotificationType::NotSeenSms, value, std::move(record))
    {}

    NotSeenCallNotification::NotSeenCallNotification(unsigned value, std::optional<ContactRecord> record)
        : NotificationWithContact(NotificationType::NotSeenCall, value, std::move(record))
    {}

    TetheringNotification::TetheringNotification() : Notification(NotificationType::Tethering)
    {}

    AlarmSnoozeNotification::AlarmSnoozeNotification(unsigned value)
        : NotificationWithCounter(NotificationType::AlarmSnooze, value)
    {}

    PhoneLockNotification::PhoneLockNotification(std::string formattedTime)
        : NotificationWithTime(NotificationType::PhoneLock, NotificationPriority::Highest, std::move(formattedTime))
    {}

    BatteryTooHotNotification::BatteryTooHotNotification()
        : Notification(NotificationType::BatteryTooHot, NotificationPriority::Highest)
    {}
} // namespace notifications
