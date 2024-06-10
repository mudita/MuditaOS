// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app::whatsNew::models
{
    class WhatsNewModel;
} // namespace app::whatsNew::models

namespace app
{
    inline constexpr auto applicationWhatsNewName      = "ApplicationWhatsNew";
    inline constexpr auto applicationWhatsNewStackSize = 1024 * 8;

    class ApplicationWhatsNew : public Application
    {
      public:
        ApplicationWhatsNew(std::string name                    = applicationWhatsNewName,
                            std::string parent                  = "",
                            StatusIndicators statusIndicators   = StatusIndicators{},
                            StartInBackground startInBackground = false,
                            std::uint32_t stackDepth            = applicationWhatsNewStackSize);
        ~ApplicationWhatsNew();
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
        std::unique_ptr<whatsNew::models::WhatsNewModel> whatsNewModel;

        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    };

    template <>
    struct ManifestTraits<ApplicationWhatsNew>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
