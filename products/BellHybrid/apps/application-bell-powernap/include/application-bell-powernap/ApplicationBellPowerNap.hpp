// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>
#include <Application.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace gui::window::name
{
    inline constexpr auto powerNapProgress     = "PowerNapProgressWindow";
    inline constexpr auto powerNapSessionEnded = "PowerNapSessionEndedWindow";
    inline constexpr auto powerNapWarning      = "PowerNapWarningWindow";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellPowerNapName = "ApplicationBellPowerNap";
    inline constexpr auto applicationBellPowerNapStackSize = 1024 * 8;
    inline constexpr std::chrono::minutes powerNapAlarmDuration{5};

    class ApplicationBellPowerNap : public Application
    {
      private:
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        void onStop() override;

      public:
        explicit ApplicationBellPowerNap(std::string name                    = applicationBellPowerNapName,
                                         std::string parent                  = "",
                                         StatusIndicators statusIndicators   = StatusIndicators{},
                                         StartInBackground startInBackground = {false},
                                         std::uint32_t stackDepth            = applicationBellPowerNapStackSize);
        ~ApplicationBellPowerNap() override;
        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler([[maybe_unused]] const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
    };

    template <>
    struct ManifestTraits<ApplicationBellPowerNap>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
