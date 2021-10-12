// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/BedtimeModel.hpp>

namespace gui::window::name
{
    inline constexpr auto bell_main_menu        = "BellMainMenu";
    inline constexpr auto bell_main_menu_dialog = "BellMainMenuDialog";
    inline constexpr auto bell_battery_shutdown = "BellBatteryShutdown";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellName = "ApplicationBell";

    class ApplicationBellMain : public Application
    {
      public:
        explicit ApplicationBellMain(std::string name                    = applicationBellName,
                                     std::string parent                  = "",
                                     StatusIndicators statusIndicators   = StatusIndicators{},
                                     StartInBackground startInBackground = {false},
                                     std::uint32_t stackDepth            = 10240);

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

      private:
        auto isHomeScreenFocused() -> bool;
        void onStart() override;
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;
        void handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data);
    };

    template <> struct ManifestTraits<ApplicationBellMain>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::ShowAlarm,
                     manager::actions::DisplayLogoAtExit,
                     manager::actions::DisplayLowBatteryScreen,
                     manager::actions::SystemBrownout}};
        }
    };
} // namespace app
