// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace gui::window::name
{
    inline constexpr auto bellSettings                  = gui::name::window::main_window;
    inline constexpr auto bellSettingsTimeUnits         = "BellSettingsTimeUnits";
    inline constexpr auto bellSettingsHomeView          = "BellSettingsHomeView";
    inline constexpr auto bellSettingsLanguage          = "BellSettingsLanguage";
    inline constexpr auto bellSettingsLayout            = "BellSettingsLayout";
    inline constexpr auto bellSettingsBedtimeTone       = "BellSettingsBedtimeTone";
    inline constexpr auto bellSettingsRelaxation        = "BellSettingsRelaxation";
    inline constexpr auto bellSettingsShortcuts         = "BellSettingsShortcuts";
    inline constexpr auto bellSettingsUpdateInstruction = "BellSettingsUpdateInstruction";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellSettingsName = "ApplicationBellSettings";

    class AbstractAudioModel;

    class ApplicationBellSettings : public Application
    {
      private:
        std::unique_ptr<AbstractAudioModel> audioModel;
        void onStop() override;

      public:
        explicit ApplicationBellSettings(std::string name                    = applicationBellSettingsName,
                                         std::string parent                  = "",
                                         StatusIndicators statusIndicators   = StatusIndicators{},
                                         StartInBackground startInBackground = {false},
                                         std::uint32_t stackDepth            = 1024 * 8);

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
    struct ManifestTraits<ApplicationBellSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
