// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>

namespace gui::window::name
{
    inline constexpr auto bellSettings                               = "BellSettings";
    inline constexpr auto bellSettingsAdvanced                       = "BellSettingsAdvanced";
    inline constexpr auto bellSettingsTimeUnits                      = "BellSettingsTimeUnits";
    inline constexpr auto bellSettingsDialog                         = "BellSettingsDialog";
    inline constexpr auto bellSettingsFinished                       = "BellSettingsFinished";
    inline constexpr auto bellSettingsFrontlight                     = "BellSettingsFrontlight";
    inline constexpr auto bellSettingsHomeView                       = "BellSettingsHomeView";
    inline constexpr auto bellSettingsAlarmSettingsAlarmToneAndLight = "BellSettingsAlarmSettingsAlarmToneAndLight";
    inline constexpr auto bellSettingsBedtimeTone                    = "BellSettingsBedtimeTone";
    inline constexpr auto bellSettingsTurnOff                        = "BellSettingsTurnOff";
} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellSettingsName = "ApplicationBellSettings";

    namespace settingsInterface
    {
        class BellScreenLightSettings
        {
          public:
            struct Values
            {
                bool lightOn;
                screen_light_control::ScreenLightMode mode;
                screen_light_control::ManualModeParameters parameters;
            };

            virtual ~BellScreenLightSettings()                  = default;
            virtual void setBrightness(float brightnessValue)   = 0;
            virtual void setMode(bool isAutoLightSwitchOn)      = 0;
            virtual void setStatus(bool isDisplayLightSwitchOn) = 0;
        };
    }; // namespace settingsInterface

    class ApplicationBellSettings : public Application, public settingsInterface::BellScreenLightSettings
    {
      public:
        ApplicationBellSettings(std::string name                    = applicationBellSettingsName,
                                std::string parent                  = "",
                                StatusIndicators statusIndicators   = StatusIndicators{},
                                StartInBackground startInBackground = {false});

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void setBrightness(float brightnessValue) override;
        void setMode(bool isAutoLightSwitchOn) override;
        void setStatus(bool isDisplayLightSwitchOn) override;
    };

    template <> struct ManifestTraits<ApplicationBellSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
