// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/ProgressBar.hpp"

namespace app
{
    const inline std::string name_clock = "ApplicationClock";

    class ApplicationClock : public Application
    {
        std::unique_ptr<sys::Timer> timerClock;
        void timerClockCallback();

      public:
        ApplicationClock(std::string name                    = name_clock,
                         std::string parent                  = {},
                         StartInBackground startInBackground = {false},
                         uint32_t stackDepth                 = 4096,
                         sys::ServicePriority priority       = sys::ServicePriority::Idle);

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationClock>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} /* namespace app */
