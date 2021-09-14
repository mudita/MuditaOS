// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationListItem.hpp"
#include "NotificationData.hpp"
#include "ListItemProvider.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include "InternalModel.hpp"
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>

namespace gui
{
    enum class NotificationsListPlacement
    {
        Desktop,
        LockedScreen
    };
    class NotificationsModel : public app::InternalModel<gui::NotificationListItem *>, public gui::ListItemProvider
    {
        [[nodiscard]] unsigned int requestRecordsCount() final;
        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const final;
        ListItem *getItem(Order order) final;
        void requestRecords(uint32_t offset, uint32_t limit) final;

      protected:
        bool tetheringOn = false;
        bool phoneTimeLock = false;
        const NotificationsListPlacement listPlacement;
        [[nodiscard]] virtual auto create(const notifications::NotSeenSMSNotification *notification)
            -> NotificationListItem *;
        [[nodiscard]] virtual auto create(const notifications::NotSeenCallNotification *notification)
            -> NotificationListItem *;
        [[nodiscard]] virtual auto create(const notifications::TetheringNotification *notification)
            -> NotificationListItem *;
        [[nodiscard]] virtual auto create(const notifications::AlarmSnoozeNotification *notification)
            -> NotificationListItem *;
        [[nodiscard]] virtual auto create(const notifications::PhoneLockNotification *notification)
            -> NotificationListItem *;

      public:
        explicit NotificationsModel(NotificationsListPlacement listPlacement = NotificationsListPlacement::Desktop);
        [[nodiscard]] bool isEmpty() const noexcept;
        [[nodiscard]] bool hasDismissibleNotification() const noexcept;
        [[nodiscard]] bool isTetheringOn() const noexcept;
        [[nodiscard]] bool isPhoneTimeLock() const noexcept;

        void updateData(app::manager::actions::NotificationsChangedParams *params);
        void dismissAll();
        void clearAll();
    };

} // namespace gui
