// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notifcationConfig);
        void registerMessageHandlers();

      private:
        void incomingCallHandler(sys::Message *request);
        void callerIdHandler(sys::Message *request);
        void incomingSMSHandler();

        void policyNumberCheck(const utils::PhoneNumber::View &number);
        void playbackCallRingtone();
        void playbackSMSRingtone();

        sys::Service *parentService = nullptr;
        NotificationsConfiguration &notifcationConfig;
        CallNotificationPolicy currentCallPolicy;
        SMSNotificationPolicy currentSMSPolicy;
    };
} // namespace notifications
