// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>

namespace gui::window::name
{
    inline constexpr auto bellSettings             = "BellSettings";
    inline constexpr auto bellSettingsTimeUnits    = "BellSettingsTimeUnits";
    inline constexpr auto bellSettingsDialog       = "BellSettingsDialog";
    inline constexpr auto bellSettingsFinished     = "BellSettingsFinished";
    inline constexpr auto bellSettingsFrontlight   = "BellSettingsFrontlight";
    inline constexpr auto bellSettingsHomeView     = "BellSettingsHomeView";
    inline constexpr auto bellSettingsLanguage     = "BellSettingsLanguage";
    inline constexpr auto bellSettingsBedtimeTone  = "BellSettingsBedtimeTone";
    inline constexpr auto bellSettingsFactoryReset = "BellSettingsFactoryReset";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellSettingsName = "ApplicationBellSettings";

    class ApplicationBellSettings : public Application
    {
      private:
        std::unique_ptr<AbstractAudioModel> audioModel;
        void onStop() override;

      public:
        ApplicationBellSettings(std::string name                    = applicationBellSettingsName,
                                std::string parent                  = "",
                                StatusIndicators statusIndicators   = StatusIndicators{},
                                StartInBackground startInBackground = {false},
                                uint32_t stackDepth                 = 4096 * 2);

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

    template <> struct ManifestTraits<ApplicationBellSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
