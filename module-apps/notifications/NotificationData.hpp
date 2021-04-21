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
        Unknown,
        NotSeenSms,
        NotSeenCall,
        Tethering
    };

    enum class NotificationPriority
    {
        Next,
        Highest,
        Lowest
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

    class NotSeenSMSNotification : public Notification
    {
        unsigned value = 0;

      public:
        explicit NotSeenSMSNotification(unsigned value);
        [[nodiscard]] auto getValue() const noexcept -> unsigned;
    };

    class NotSeenCallNotification : public Notification
    {
        unsigned value = 0;
        std::unique_ptr<ContactRecord> record;

      public:
        explicit NotSeenCallNotification(unsigned value, std::unique_ptr<ContactRecord> record = nullptr);

        [[nodiscard]] bool hasRecord() const noexcept;
        [[nodiscard]] auto getRecord() const noexcept -> const std::unique_ptr<ContactRecord> &;
        [[nodiscard]] auto getValue() const noexcept -> unsigned;
    };

    class TetheringNotification : public Notification
    {
      public:
        TetheringNotification();
    };

} // namespace notifications
