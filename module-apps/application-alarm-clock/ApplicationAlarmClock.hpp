// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"

namespace app
{
    inline constexpr auto name_alarm_clock = "ApplicationAlarmClock";

    class ApplicationAlarmClock : public Application
    {

      public:
        ApplicationAlarmClock(std::string name,
                              std::string parent,
                              sys::phone_modes::PhoneMode phoneMode       = sys::phone_modes::PhoneMode::Connected,
                              sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
                              uint32_t stackDepth                         = 4096 * 2,
                              sys::ServicePriority priority               = sys::ServicePriority::Idle);

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationAlarmClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {
                {manager::actions::Launch, manager::actions::PhoneModeChanged, manager::actions::BluetoothModeChanged}};
        }
    };
} // namespace app
