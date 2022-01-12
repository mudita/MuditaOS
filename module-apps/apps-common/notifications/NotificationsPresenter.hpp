// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationData.hpp"
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>

namespace gui
{
    class NotificationsPresenter
    {
      public:
        NotificationsPresenter()                                                       = default;
        [[nodiscard]] virtual auto isEmpty() const noexcept -> bool                    = 0;
        [[nodiscard]] virtual auto hasDismissibleNotification() const noexcept -> bool = 0;
        virtual void updateData(app::manager::actions::NotificationsChangedParams *params,
                                bool callAndSMSVisibility)                             = 0;
        virtual void dismissAll()                                                      = 0;
    };

} // namespace gui
