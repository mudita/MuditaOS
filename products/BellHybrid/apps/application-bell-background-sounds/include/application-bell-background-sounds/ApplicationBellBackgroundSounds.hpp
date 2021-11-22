// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>

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
      private:
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<bgSounds::BGSoundsPlayer> player;

        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;

      public:
        ApplicationBellBackgroundSounds(std::string name                    = applicationBellBackgroundSoundsName,
                                        std::string parent                  = "",
                                        StatusIndicators statusIndicators   = StatusIndicators{},
                                        StartInBackground startInBackground = {false},
                                        uint32_t stackDepth                 = 4096 * 2);
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
