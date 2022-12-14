// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "Constants.hpp"

namespace gui::window::name
{
    inline constexpr auto onBoarding_languages                = "OnBoardingLanguages";
    inline constexpr auto onBoarding_start_configuration      = "OnBoardingStartingConfiguration";
    inline constexpr auto onBoarding_eula                     = "OnBoardingEula";
    inline constexpr auto onBoarding_configuration_successful = "OnBoardingConfigurationSuccessful";
    inline constexpr auto onBoarding_no_configuration         = "OnBoardingNoConfiguration";
    inline constexpr auto onBoarding_update                   = "OnBoardingUpdate";
    inline constexpr auto onBoarding_skip                     = "OnBoardingSkipConfirm";
    inline constexpr auto onBoarding_date_and_time            = "OnBoardingDateAndTime";
    inline constexpr auto onBoarding_change_date_and_time     = "OnBoardingChangeDateAndTime";
    inline constexpr auto onBoarding_sim_select               = "OnBoardingSimSelect";
    inline constexpr auto onBoarding_no_sim_selected          = "OnBoardingNoSimSelected";
} // namespace gui::window::name

namespace app
{
    class ApplicationOnBoarding : public Application
    {
      public:
        explicit ApplicationOnBoarding(std::string name                    = name_onboarding,
                                       std::string parent                  = {},
                                       StatusIndicators statusIndicators   = StatusIndicators{},
                                       StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

        void acceptEULA();
        void finalizeOnBoarding();

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <>
    struct ManifestTraits<ApplicationOnBoarding>
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
