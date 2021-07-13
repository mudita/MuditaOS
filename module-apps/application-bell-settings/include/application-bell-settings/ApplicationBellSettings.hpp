// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bellSettings          = "BellSettings";
    inline constexpr auto bellSettingsAdvanced  = "BellSettingsAdvanced";
    inline constexpr auto bellSettingsTimeUnits = "BellSettingsTimeUnits";
    inline constexpr auto bellSettingsDialog    = "BellSettingsDialog";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellSettingsName = "ApplicationBellSettings";

    class ApplicationBellSettings : public Application
    {
      public:
        ApplicationBellSettings(std::string name                    = applicationBellSettingsName,
                                std::string parent                  = "",
                                sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Offline,
                                StartInBackground startInBackground = {false});

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

    template <> struct ManifestTraits<ApplicationBellSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
