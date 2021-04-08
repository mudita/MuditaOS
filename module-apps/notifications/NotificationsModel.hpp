// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationListItem.hpp"
#include "NotificationData.hpp"
#include "ListItemProvider.hpp"

#include "Application.hpp"
#include "InternalModel.hpp"
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>

namespace gui
{

    class NotificationsModel : public app::InternalModel<gui::NotificationListItem *>, public gui::ListItemProvider
    {
        [[nodiscard]] unsigned int requestRecordsCount() final;
        [[nodiscard]] unsigned int getMinimalItemHeight() const final;
        ListItem *getItem(Order order) final;
        void requestRecords(uint32_t offset, uint32_t limit) final;

      protected:
        AppWindow *parent = nullptr;
        [[nodiscard]] virtual auto create(const notifications::NotSeenSMSNotification *notification)
            -> NotificationListItem *;
        [[nodiscard]] virtual auto create(const notifications::NotSeenCallNotification *notification)
            -> NotificationListItem *;

      public:
        [[nodiscard]] bool isEmpty() const noexcept;
        [[nodiscard]] bool hasDismissibleNotification() const noexcept;

        void updateData(app::manager::actions::NotificationsChangedParams *params);
        void dismissAll(const InputEvent &event);
        void clearAll();
        void setParentWindow(AppWindow *parentWindow) noexcept;
    };

} // namespace gui
