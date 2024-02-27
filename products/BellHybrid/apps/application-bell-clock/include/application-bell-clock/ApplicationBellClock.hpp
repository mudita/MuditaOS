// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bellClock = "BellClock";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellClockName = "ApplicationBellClock";

    class ApplicationBellClock : public Application
    {
      public:
        explicit ApplicationBellClock(std::string name                    = applicationBellClockName,
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
    struct ManifestTraits<ApplicationBellClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
