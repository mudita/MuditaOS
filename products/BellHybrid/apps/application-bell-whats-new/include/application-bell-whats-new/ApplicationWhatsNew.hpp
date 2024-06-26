// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace app::whatsnew::models
{
    class WhatsNewFeaturesModel;
}

namespace app
{
    class AbstractBatteryModel;
    class AbstractLowBatteryInfoModel;
} // namespace app

namespace app
{
    inline constexpr auto applicationWhatsNewName      = "ApplicationWhatsNew";
    inline constexpr auto applicationWhatsNewStackSize = 1024 * 14;

    class ApplicationWhatsNew : public Application
    {
      public:
        explicit ApplicationWhatsNew(std::string name                    = applicationWhatsNewName,
                                     std::string parent                  = "",
                                     StatusIndicators statusIndicators   = StatusIndicators{},
                                     StartInBackground startInBackground = false,
                                     std::uint32_t stackDepth            = applicationWhatsNewStackSize);
        ~ApplicationWhatsNew() override;

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
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        std::unique_ptr<whatsnew::models::WhatsNewFeaturesModel> featuresModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
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
