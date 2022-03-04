// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::name::window
{
    inline constexpr auto intervalChime      = "IntervalChimeWindow";
    inline constexpr auto meditationProgress = "MeditationProgressWindow";
    inline constexpr auto meditationTimer    = "MeditationTimerWindow";
    inline constexpr auto readyGoing         = "ReadyGoingWindow";
    inline constexpr auto sessionEnded       = "SessionEndedWindow";
} // namespace gui::name::window

namespace app
{
    inline constexpr auto applicationBellMeditationTimerName = "ApplicationBellMeditationTimer";

    class ApplicationBellMeditationTimer : public Application
    {
      public:
        ApplicationBellMeditationTimer(std::string name                    = applicationBellMeditationTimerName,
                                       std::string parent                  = "",
                                       StatusIndicators statusIndicators   = StatusIndicators{},
                                       StartInBackground startInBackground = {false},
                                       uint32_t stackDepth                 = 4096 * 2);
        ~ApplicationBellMeditationTimer();

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

    template <> struct ManifestTraits<ApplicationBellMeditationTimer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
