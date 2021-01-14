// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"

namespace app
{
    inline constexpr auto name_alarm_clock = "ApplicationAlarmClock";

    class ApplicationAlarmClock : public Application
    {

        audio::Token audioToken;

      public:
        ApplicationAlarmClock(std::string name,
                              std::string parent,
                              uint32_t stackDepth           = 4096,
                              sys::ServicePriority priority = sys::ServicePriority::Idle);

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        audio::Token getAudioToken() const;
    };

    template <> struct ManifestTraits<ApplicationAlarmClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::ShowAlarm}};
        }
    };
} // namespace app
