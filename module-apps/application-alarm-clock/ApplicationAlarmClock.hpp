// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include <apps-common/audio/SoundsPlayer.hpp>

class AudioStopNotification;

namespace app
{
    inline constexpr auto name_alarm_clock = "ApplicationAlarmClock";

    class ApplicationAlarmClock : public Application
    {

      public:
        ApplicationAlarmClock(std::string name,
                              std::string parent,
                              StatusIndicators statusIndicators = StatusIndicators{},
                              uint32_t stackDepth               = 4096 * 2,
                              sys::ServicePriority priority     = sys::ServicePriority::Idle);

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

      private:
        sys::MessagePointer handleAudioStop(AudioStopNotification *notification);
        std::shared_ptr<SoundsPlayer> soundsPlayer;
    };

    template <> struct ManifestTraits<ApplicationAlarmClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged}};
        }
    };
} // namespace app
