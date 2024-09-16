// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app::focus::models
{
    class FocusSettingsModel;
    class NotificationVolumeModel;
} // namespace app::focus::models

namespace app
{
    inline constexpr auto applicationFocusTimerName      = "ApplicationFocusTimer";
    inline constexpr auto applicationFocusTimerStackSize = 1024 * 8;

    class ApplicationFocusTimer : public Application
    {
      public:
        explicit ApplicationFocusTimer(std::string name                    = applicationFocusTimerName,
                                       std::string parent                  = "",
                                       StatusIndicators statusIndicators   = StatusIndicators{},
                                       StartInBackground startInBackground = {false},
                                       std::uint32_t stackDepth            = applicationFocusTimerStackSize);
        ~ApplicationFocusTimer();
        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler([[maybe_unused]] const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

      private:
        std::unique_ptr<focus::models::FocusSettingsModel> focusTimeModel;
        std::unique_ptr<focus::models::FocusSettingsModel> focusRepeatsModel;
        std::unique_ptr<focus::models::FocusSettingsModel> shortBreakTimeModel;
        std::unique_ptr<focus::models::FocusSettingsModel> longBreakTimeModel;
        std::unique_ptr<focus::models::FocusSettingsModel> longBreakOccurrenceModel;
        std::unique_ptr<focus::models::NotificationVolumeModel> notificationVolumeModel;

        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    };

    template <>
    struct ManifestTraits<ApplicationFocusTimer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
