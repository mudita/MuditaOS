// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/ProgressBar.hpp"
#include "module-sys/Timers/TimerHandle.hpp"

namespace app
{
    const inline std::string name_clock = "ApplicationClock";

    class ApplicationClock : public Application
    {
        sys::TimerHandle timerClock;
        void timerClockCallback();

      public:
        explicit ApplicationClock(std::string name                            = name_clock,
                                  std::string parent                          = {},
                                  sys::phone_modes::PhoneMode phoneMode       = sys::phone_modes::PhoneMode::Connected,
                                  sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
                                  StartInBackground startInBackground         = {false},
                                  uint32_t stackDepth                         = 4096,
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

    template <> struct ManifestTraits<ApplicationClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {
                {manager::actions::Launch, manager::actions::PhoneModeChanged, manager::actions::BluetoothModeChanged}};
        }
    };
} /* namespace app */
