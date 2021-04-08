// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>
#include <list>

#include <ContactRecord.hpp>
#include <Service/Message.hpp>

namespace notifications
{
    enum class NotificationType
    {
        Unknown,
        NotSeenSms,
        NotSeenCall,
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
        explicit Notification(NotificationType type, NotificationPriority priorityType = NotificationPriority::Next)
            : type{type}
        {
            switch (priorityType) {
            case NotificationPriority::Next:
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

      public:
        [[nodiscard]] auto getType() const noexcept
        {
            return type;
        }

        [[nodiscard]] auto getPriority() const noexcept
        {
            return priority;
        }

        virtual ~Notification() = default;
    };

    class NotSeenSMSNotification : public Notification
    {
        unsigned value = 0;

      public:
        explicit NotSeenSMSNotification(unsigned value) : Notification(NotificationType::NotSeenSms), value{value}
        {}
        [[nodiscard]] auto getValue() const noexcept
        {
            return value;
        }
    };

    class NotSeenCallNotification : public Notification
    {
        unsigned value = 0;
        std::unique_ptr<ContactRecord> record;

      public:
        explicit NotSeenCallNotification(unsigned value, std::unique_ptr<ContactRecord> record = nullptr)
            : Notification(NotificationType::NotSeenCall), value{value}, record{std::move(record)}
        {}

        [[nodiscard]] bool hasRecord() const noexcept
        {
            return record != nullptr;
        }

        [[nodiscard]] auto getRecord() const noexcept -> const std::unique_ptr<ContactRecord> &
        {
            return record;
        }

        [[nodiscard]] auto getValue() const noexcept
        {
            return value;
        }
    };

} // namespace notifications
