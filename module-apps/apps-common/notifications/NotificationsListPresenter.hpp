// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationsPresenter.hpp"
#include "NotificationListItem.hpp"
#include "NotificationData.hpp"
#include "ListItemProvider.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include "InternalModel.hpp"
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>

namespace gui
{
    class NotificationsListPresenter : public NotificationsPresenter,
                                       public app::InternalModel<gui::NotificationListItem *>,
                                       public gui::ListItemProvider
    {
        [[nodiscard]] unsigned int requestRecordsCount() final;
        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const final;
        ListItem *getItem(Order order) final;
        void requestRecords(uint32_t offset, uint32_t limit) final;

      protected:
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
        NotificationsListPresenter();
        [[nodiscard]] bool isEmpty() const noexcept final;
        [[nodiscard]] bool hasDismissibleNotification() const noexcept final;

        void updateData(app::manager::actions::NotificationsChangedParams *params, bool callAndSMSVisibility) final;
        void dismissAll() final;
    };

} // namespace gui
