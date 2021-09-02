// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include <SystemManager/SystemManagerCommon.hpp>

namespace app
{

    inline constexpr auto name_calendar = "ApplicationCalendar";

    class ApplicationCalendar : public Application
    {
      public:
        ApplicationCalendar(std::string name,
                            std::string parent,
                            sys::phone_modes::PhoneMode phoneMode       = sys::phone_modes::PhoneMode::Connected,
                            sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
                            StartInBackground startInBackground         = {false},
                            uint32_t stackDepth                         = 8192,
                            sys::ServicePriority priority               = sys::ServicePriority::Idle);

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationCalendar>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {
                {manager::actions::Launch, manager::actions::PhoneModeChanged, manager::actions::BluetoothModeChanged}};
        }
    };
} /* namespace app */
