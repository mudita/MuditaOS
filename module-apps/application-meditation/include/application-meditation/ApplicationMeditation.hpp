// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-meditation/OptionsData.hpp>

#include <Application.hpp>

#include <string>

namespace app
{
    inline constexpr auto name_meditation = "ApplicationMeditation";

    class ApplicationMeditation : public Application
    {
      public:
        explicit ApplicationMeditation(std::string name                    = name_meditation,
                                       std::string parent                  = {},
                                       StatusIndicators statusIndicators   = StatusIndicators{},
                                       StartInBackground startInBackground = {false});

        auto InitHandler() -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;
        auto SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes final;

        void createUserInterface() override;
        void destroyUserInterface() override;
        std::unique_ptr<gui::OptionsData> state;
    };

    template <>
    struct ManifestTraits<ApplicationMeditation>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged},
                    locks::AutoLockPolicy::PreventPermanently};
        }
    };
} // namespace app
