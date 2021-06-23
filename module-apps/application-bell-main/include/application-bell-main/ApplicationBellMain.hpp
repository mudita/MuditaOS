// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bell_main_menu        = "BellMainMenu";
    inline constexpr auto bell_main_menu_dialog = "BellMainMenuDialog";
    inline constexpr auto bell_main_set_hour    = "BellMainSetHour";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellName = "ApplicationBell";

    class ApplicationBellMain : public Application
    {
      public:
        ApplicationBellMain(std::string name                    = applicationBellName,
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

    template <> struct ManifestTraits<ApplicationBellMain>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
