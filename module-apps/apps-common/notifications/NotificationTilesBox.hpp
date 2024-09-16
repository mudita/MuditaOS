// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

#include "NotificationData.hpp"
#include <notifications/NotificationTilesPresenter.hpp>

namespace gui
{
    class NotificationTilesBox : public HBox
    {
      public:
        explicit NotificationTilesBox(Item *parent, std::shared_ptr<NotificationTilesPresenter> notificationsPresenter);

      private:
        std::shared_ptr<NotificationTilesPresenter> notificationsPresenter;
    };

} // namespace gui
