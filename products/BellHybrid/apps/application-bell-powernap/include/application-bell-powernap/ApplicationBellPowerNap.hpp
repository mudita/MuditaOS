// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto powernapProgress     = "PowerNapProgressWindow";
    inline constexpr auto powernapSessionEnded = "PowerNapSessionEndedWindow";
} // namespace gui::window::name
namespace app
{
    namespace powernap
    {
        class PowerNapAlarmImpl;
    }

    inline constexpr auto applicationBellPowerNapName = "ApplicationBellPowerNap";

    class ApplicationBellPowerNap : public Application
    {
      public:
        ApplicationBellPowerNap(std::string name                    = applicationBellPowerNapName,
                                std::string parent                  = "",
                                StatusIndicators statusIndicators   = StatusIndicators{},
                                StartInBackground startInBackground = {false},
                                uint32_t stackDepth                 = 4096 * 2);
        ~ApplicationBellPowerNap();
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
        void onStart() override;
    };

    template <> struct ManifestTraits<ApplicationBellPowerNap>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
