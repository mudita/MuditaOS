// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bgSoundsPaused      = "BGSoundsPausedWindow";
    inline constexpr auto bgSoundsProgress    = "BGSoundsProgressWindow";
    inline constexpr auto bgSoundsTimerSelect = "BGSoundsTimerSelectWindow";
} // namespace gui::window::name
namespace app
{
    namespace bgSounds
    {
        class BGSoundsPlayer;
    }
    inline constexpr auto applicationBellBackgroundSoundsName = "ApplicationBellBackgroundSounds";

    class ApplicationBellBackgroundSounds : public Application
    {
        std::unique_ptr<bgSounds::BGSoundsPlayer> player;

      public:
        ApplicationBellBackgroundSounds(std::string name                    = applicationBellBackgroundSoundsName,
                                        std::string parent                  = "",
                                        StatusIndicators statusIndicators   = StatusIndicators{},
                                        StartInBackground startInBackground = {false});
        ~ApplicationBellBackgroundSounds();
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

    template <> struct ManifestTraits<ApplicationBellBackgroundSounds>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
