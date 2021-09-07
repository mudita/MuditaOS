// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <CalllogRecord.hpp>
#include <OptionWindow.hpp>

namespace app
{

    inline constexpr auto CallLogAppStr = "ApplicationCallLog";

    class ApplicationCallLog : public Application
    {
      public:
        ApplicationCallLog(std::string name                            = CallLogAppStr,
                           std::string parent                          = {},
                           sys::phone_modes::PhoneMode phoneMode       = sys::phone_modes::PhoneMode::Connected,
                           sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
                           StartInBackground startInBackground         = {false});
        ~ApplicationCallLog() override;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() final;
        void destroyUserInterface() final;

        bool removeCalllogEntry(const CalllogRecord &record);

        bool setAllEntriesRead();
    };

    template <> struct ManifestTraits<ApplicationCallLog>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::ShowCallLog,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged}};
        }
    };
} /* namespace app */
