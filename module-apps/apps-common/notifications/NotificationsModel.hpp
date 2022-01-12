// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationsPresenter.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>

namespace gui
{
    enum class NotificationsListPlacement
    {
        Desktop,
        LockedScreen
    };
    class NotificationsModel
    {
      private:
        std::shared_ptr<NotificationsPresenter> notificationsPresenter;
        bool tetheringOn = false;
        bool phoneTimeLock = false;
        const NotificationsListPlacement listPlacement;

      public:
        explicit NotificationsModel(std::shared_ptr<NotificationsPresenter> notificationsPresenter,
                                    NotificationsListPlacement listPlacement = NotificationsListPlacement::Desktop);
        explicit NotificationsModel(NotificationsListPlacement listPlacement = NotificationsListPlacement::Desktop);
        void attachPresenter(std::shared_ptr<NotificationsPresenter> notificationsPresenter);
        [[nodiscard]] bool isEmpty() const noexcept;
        [[nodiscard]] bool hasDismissibleNotification() const noexcept;
        [[nodiscard]] bool isTetheringOn() const noexcept;
        [[nodiscard]] bool isPhoneTimeLock() const noexcept;

        void updateData(app::manager::actions::NotificationsChangedParams *params);
        void dismissAll();
    };

} // namespace gui
