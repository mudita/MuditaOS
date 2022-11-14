// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <purefs/filesystem_paths.hpp>

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
    namespace relaxation
    {
        class RelaxationPlayer;
    }
    inline constexpr auto applicationBellRelaxationName = "ApplicationBellRelaxation";

    class ApplicationBellRelaxation : public Application
    {
      private:
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<relaxation::RelaxationPlayer> player;

        void onStop() override;
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;

      public:
        ApplicationBellRelaxation(std::string name                    = applicationBellRelaxationName,
                                  std::string parent                  = "",
                                  StatusIndicators statusIndicators   = StatusIndicators{},
                                  StartInBackground startInBackground = {false},
                                  uint32_t stackDepth                 = 4096 * 2);
        ~ApplicationBellRelaxation();
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

    template <>
    struct ManifestTraits<ApplicationBellRelaxation>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
