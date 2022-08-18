// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bellBedtime      = "BellBedtime";
    inline constexpr auto bellBedtimeOnOff = "bellBedtimeOnff";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellBedtimeName = "ApplicationBellBedtime";

    class ApplicationBellBedtime : public Application
    {
      public:
        explicit ApplicationBellBedtime(std::string name                    = applicationBellBedtimeName,
                                        std::string parent                  = "",
                                        StatusIndicators statusIndicators   = StatusIndicators{},
                                        StartInBackground startInBackground = {false},
                                        uint32_t stackDepth                 = 4096 * 2);

        auto InitHandler() -> sys::ReturnCodes override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes override final
        {
            return sys::ReturnCodes::Success;
        }
    };

    template <>
    struct ManifestTraits<ApplicationBellBedtime>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
