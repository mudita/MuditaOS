// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ApplicationBellAlarmNames.hpp"
#include <Application.hpp>

namespace app
{
    inline constexpr auto applicationBellAlarmName = "ApplicationBellAlarm";

    class ApplicationBellAlarm : public Application
    {
      public:
        explicit ApplicationBellAlarm(std::string name                    = applicationBellAlarmName,
                                      std::string parent                  = "",
                                      StatusIndicators statusIndicators   = StatusIndicators{},
                                      StartInBackground startInBackground = {false},
                                      std::uint32_t stackDepth            = 1024 * 8);

        ~ApplicationBellAlarm() override;
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
    struct ManifestTraits<ApplicationBellAlarm>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
