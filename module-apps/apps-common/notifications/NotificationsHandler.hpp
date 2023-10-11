// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NotificationsConfiguration.hpp"
#include "policies/CallNotificationPolicy.hpp"
#include "policies/SMSNotificationPolicy.hpp"
#include <PhoneModes/Observer.hpp>
#include <Service/Service.hpp>
#include <PhoneNumber.hpp>

namespace notifications
{
    class NotificationsHandler
    {
      public:
        NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notificationConfig);
        void registerMessageHandlers();

      private:
        void incomingSMSHandler();
        void playbackSMSRingtone();

        sys::Service *parentService = nullptr;
        NotificationsConfiguration &notificationConfig;
        SMSNotificationPolicy currentSMSPolicy;
    };
} // namespace notifications
