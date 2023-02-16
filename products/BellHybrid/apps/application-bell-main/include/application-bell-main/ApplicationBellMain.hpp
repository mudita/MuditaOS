// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenters/HomeScreenPresenter.hpp"

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
                                     std::uint32_t stackDepth            = 1024 * 15);
        ~ApplicationBellMain();

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
        void onStart() override;
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;
        bool setHomeScreenLayout(std::string layoutName);

        std::unique_ptr<AbstractTimeModel> timeModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<home_screen::AbstractTemperatureModel> temperatureModel;
        std::shared_ptr<app::home_screen::HomeScreenPresenter> homeScreenPresenter;
    };

    template <>
    struct ManifestTraits<ApplicationBellMain>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::ShowAlarm,
                     manager::actions::DisplayLogoAtExit,
                     manager::actions::DisplayChargeAtExit,
                     manager::actions::SystemBrownout,
                     {manager::actions::ChangeHomescreenLayout, manager::actions::ActionFlag::AcceptWhenInBackground}}};
        }
    };
} // namespace app
