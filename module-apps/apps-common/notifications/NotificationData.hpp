// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>
#include <list>

#include <ContactRecord.hpp>

namespace notifications
{
    enum class NotificationType
    {
        NotSeenSms,
        NotSeenCall,
        Tethering,
        AlarmSnooze,
        PhoneLock
    };

    enum class NotificationPriority
    {
        Next,
        Highest,
        Lowest
    };

    enum class NotificationOnReceiveUpdate
    {
        PartialRebuild,
        FullRebuild,
    };

    class Notification
    {
        static constexpr auto highestPriority = std::numeric_limits<uint32_t>::max();
        static constexpr auto lowestPriority  = 0;
        static uint32_t priorityPool;

        NotificationType type;
        uint32_t priority;

      protected:
        explicit Notification(NotificationType type, NotificationPriority priorityType = NotificationPriority::Next);

      public:
        [[nodiscard]] auto getType() const noexcept -> NotificationType;
        [[nodiscard]] auto getPriority() const noexcept -> uint32_t;

        virtual ~Notification() = default;
    };

    class NotificationWithContact : public Notification
    {
        unsigned value = 0;
        std::optional<ContactRecord> record;

      protected:
        NotificationWithContact(NotificationType type, unsigned value, std::optional<ContactRecord> record);

      public:
        [[nodiscard]] auto hasRecord() const noexcept -> bool;
        [[nodiscard]] auto getRecord() const noexcept -> const ContactRecord &;
        [[nodiscard]] auto getValue() const noexcept -> unsigned;
    };

    class NotificationWithCounter : public Notification
    {
        unsigned value = 0;

      protected:
        explicit NotificationWithCounter(NotificationType type, unsigned value);

      public:
        [[nodiscard]] auto getValue() const noexcept -> unsigned;
    };

    class NotificationWithTime : public Notification
    {
        std::string formattedTime;

      protected:
        NotificationWithTime(NotificationType type, NotificationPriority priorityType, std::string formattedTime);

      public:
        [[nodiscard]] auto getTime() const noexcept -> const std::string &;
    };

    class NotSeenSMSNotification : public NotificationWithContact
    {
      public:
        NotSeenSMSNotification(unsigned value, std::optional<ContactRecord> record);
    };

    class NotSeenCallNotification : public NotificationWithContact
    {
      public:
        NotSeenCallNotification(unsigned value, std::optional<ContactRecord> record);
    };

    class TetheringNotification : public Notification
    {
      public:
        TetheringNotification();
    };

    class AlarmSnoozeNotification : public NotificationWithCounter
    {
      public:
        explicit AlarmSnoozeNotification(unsigned snoozeCount);
    };

    class PhoneLockNotification : public NotificationWithTime
    {
      public:
        explicit PhoneLockNotification(std::string formattedTime);
    };

} // namespace notifications
