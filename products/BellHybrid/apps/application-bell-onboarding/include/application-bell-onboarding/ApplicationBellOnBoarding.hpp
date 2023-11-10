// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BellOnBoardingNames.hpp"
#include <Application.hpp>

namespace app::OnBoarding
{
    /// Image name, Prompt info text
    using InformationDisplay = std::pair<std::string, std::string>;

    enum class InformationStates
    {
        RotateInfo,
        LightClickInfo,
        DeepClickWarningInfo,
        DeepClickCorrectionInfo
    };
} // namespace app::OnBoarding

namespace app
{
    class ApplicationBellOnBoarding : public Application
    {
      private:
        OnBoarding::InformationStates informationState = OnBoarding::InformationStates::RotateInfo;
        sys::TimerHandle informationPromptTimer;
        sys::TimerHandle userIdleTimer;

        OnBoarding::InformationDisplay getDisplayDataFromState();
        void displayInformation(const std::string &windowToReturn);
        bool isInformationPromptPermittedOnCurrentWindow();
        void startTimerOnWindows();
        void restartTimerOnWindows();
        bool handleInformationOnInputEvent(sys::DataMessage *msgl);
        void handleInformationBeforeSwitchWindow(sys::DataMessage *msgl);
        void handleInformationAfterSwitchWindow();
        void onStart() override;

      public:
        explicit ApplicationBellOnBoarding(std::string name                    = applicationBellOnBoardingName,
                                           std::string parent                  = "",
                                           StatusIndicators statusIndicators   = StatusIndicators{},
                                           StartInBackground startInBackground = {false},
                                           std::uint32_t stackDepth            = 1024 * 11);
        ~ApplicationBellOnBoarding();

        sys::ReturnCodes InitHandler() override;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final;
        void createUserInterface() override;
        void destroyUserInterface() override;

        void finalizeOnBoarding();
    };

    template <>
    struct ManifestTraits<ApplicationBellOnBoarding>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
