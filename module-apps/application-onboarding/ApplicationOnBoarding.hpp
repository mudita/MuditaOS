// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

namespace gui::window::name
{
    inline constexpr auto onBoarding_languages           = "OnBoardingLanguages";
    inline constexpr auto onBoarding_start_configuration = "OnBoardingStartingConfiguration";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto name_onboarding = "ApplicationOnBoarding";

    class ApplicationOnBoarding : public Application
    {
      public:
        explicit ApplicationOnBoarding(std::string name                    = name_onboarding,
                                       std::string parent                  = {},
                                       StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationOnBoarding>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
