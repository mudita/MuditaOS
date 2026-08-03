// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "NotificationsConfiguration.hpp"
#include "policies/CallNotificationPolicy.hpp"
#include "policies/SMSNotificationPolicy.hpp"
#include <module-db/queries/notifications/QuerySMSAlertOverrideGetByNumber.hpp>
#include <PhoneModes/Observer.hpp>
#include <Service/Service.hpp>
#include <PhoneNumber.hpp>
#include <Timers/TimerHandle.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace notifications
{
    class NotificationsHandler
    {
      public:
        NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notificationConfig);
        void registerMessageHandlers();

      private:
        struct ActiveSMSAlert
        {
          db::query::notifications::SMSAlertOverride overrideConfig;
          std::uint32_t initialUnreadCount = 0;
          std::optional<audio::Volume> previousVolume;
          bool volumeOverridden = false;
        };

        void incomingSMSHandler(const utils::PhoneNumber::View &senderNumber);
        void playbackSMSRingtone(std::optional<std::string> soundPathOverride);
        void playEmergencySignal(const db::query::notifications::SMSAlertOverride &overrideConfig);
        void startRepeatAlertLoop(const db::query::notifications::SMSAlertOverride &overrideConfig);
        void stopRepeatAlertLoop();
        void repeatAlertHandler();
        void startContinuousVibration();
        void stopContinuousVibration();
        void vibrationPulseHandler();
        auto getUnreadSMSCount() const -> std::optional<std::uint32_t>;
        auto getSMSAlertOverride(const utils::PhoneNumber::View &senderNumber) const
            -> std::optional<db::query::notifications::SMSAlertOverride>;

        sys::Service *parentService = nullptr;
        NotificationsConfiguration &notificationConfig;
        SMSNotificationPolicy currentSMSPolicy;
        sys::TimerHandle repeatAlertTimer;
        sys::TimerHandle vibrationAlertTimer;
        std::optional<ActiveSMSAlert> activeSMSAlert;
    };
} // namespace notifications
