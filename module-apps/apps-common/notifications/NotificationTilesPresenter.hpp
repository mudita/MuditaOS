// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationsPresenter.hpp"

#include <BoxLayout.hpp>

namespace gui
{
    class NotificationTilesPresenter : public NotificationsPresenter
    {
      public:
        NotificationTilesPresenter();
        void attach(HBox *tilesBox);
        [[nodiscard]] bool isEmpty() const noexcept final;
        [[nodiscard]] bool hasDismissibleNotification() const noexcept final;

        void updateData(app::manager::actions::NotificationsChangedParams *params, bool callAndSMSVisibility) final;
        void dismissAll() final;

      private:
        HBox *tilesBox = nullptr;
    };

} // namespace gui
