// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bgSoundsVolume = "BGSoundsVolumeWindow";
}
namespace app
{
    inline constexpr auto applicationBellBackgroundSoundsName = "ApplicationBellBackgroundSounds";

    class ApplicationBellBackgroundSounds : public Application
    {
      public:
        ApplicationBellBackgroundSounds(
            std::string name                            = applicationBellBackgroundSoundsName,
            std::string parent                          = "",
            sys::phone_modes::PhoneMode mode            = sys::phone_modes::PhoneMode::Offline,
            sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
            StartInBackground startInBackground         = {false});

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
    };

    template <> struct ManifestTraits<ApplicationBellBackgroundSounds>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
