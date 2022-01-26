// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "policies/CallNotificationPolicy.hpp"
#include "policies/NotificationsListPolicy.hpp"
#include "policies/SMSNotificationPolicy.hpp"
#include <service-db/Settings.hpp>
#include <PhoneModes/Observer.hpp>
#include <apps-common/locks/handlers/PhoneLockHandler.hpp>

namespace notifications
{
    class NotificationsConfiguration
    {
      public:
        NotificationsConfiguration(std::shared_ptr<sys::phone_modes::Observer> phoneModeObserver,
                                   std::shared_ptr<settings::Settings> settings,
                                   const locks::PhoneLockHandler &phoneLockHandler);
        void updateCurrentCall(CallNotificationPolicy &policy);
        void updateCurrentList(NotificationsListPolicy &policy);
        void updateCurrentSMS(SMSNotificationPolicy &policy);

      private:
        auto getDNDNotificationsOnLockedScreen() const noexcept -> bool;
        auto getDNDCallsFromFavourites() const noexcept -> bool;
        std::shared_ptr<sys::phone_modes::Observer> phoneModeObserver;
        std::shared_ptr<settings::Settings> settings;
        const locks::PhoneLockHandler &phoneLockHandler;
    };
} // namespace notifications
