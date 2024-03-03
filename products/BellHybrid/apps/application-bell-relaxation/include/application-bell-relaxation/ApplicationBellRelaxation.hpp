// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto relaxationPaused          = "RelaxationPausedWindow";
    inline constexpr auto relaxationRunningProgress = "RelaxationRunningProgressWindow";
    inline constexpr auto relaxationRunningLoop     = "RelaxationRunningLoopWindow";
    inline constexpr auto relaxationTimerSelect     = "RelaxationTimerSelectWindow";
    inline constexpr auto relaxationEnded           = "RelaxationEndedWindow";
    inline constexpr auto relaxationLowBattery      = "RelaxationLowBatteryWindow";
} // namespace gui::window::name

namespace app
{
    class AbstractAudioModel;
    class AbstractLowBatteryInfoModel;
    class AbstractRelaxationFadeModel;

    namespace relaxation
    {
        class RelaxationPlayer;
    }

    inline constexpr auto applicationBellRelaxationName = "ApplicationBellRelaxation";

    class ApplicationBellRelaxation : public Application
    {
      public:
        explicit ApplicationBellRelaxation(std::string name                    = applicationBellRelaxationName,
                                           std::string parent                  = "",
                                           StatusIndicators statusIndicators   = StatusIndicators{},
                                           StartInBackground startInBackground = {false},
                                           std::uint32_t stackDepth            = 1024 * 8);
        ~ApplicationBellRelaxation() override;
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
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractRelaxationFadeModel> fadeModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
        std::unique_ptr<relaxation::RelaxationPlayer> player;
        sys::TimerHandle relaxationRebuildTimerHandle{};
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;

        void onStop() override;
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;
    };

    template <>
    struct ManifestTraits<ApplicationBellRelaxation>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
