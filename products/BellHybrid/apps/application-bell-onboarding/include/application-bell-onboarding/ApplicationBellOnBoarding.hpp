// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BellOnBoardingNames.hpp"
#include <Application.hpp>

namespace app
{
    class ApplicationBellOnBoarding : public Application
    {
      public:
        ApplicationBellOnBoarding(std::string name                            = applicationBellOnBoardingName,
                                  std::string parent                          = "",
                                  sys::phone_modes::PhoneMode mode            = sys::phone_modes::PhoneMode::Offline,
                                  sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled,
                                  StartInBackground startInBackground         = {false});

        sys::ReturnCodes InitHandler() override;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final;
        void createUserInterface() override;
        void destroyUserInterface() override;

        void finalizeOnBoarding();
    };

    template <> struct ManifestTraits<ApplicationBellOnBoarding>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
