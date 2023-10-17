// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/apps/AudioSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/windows/OptionsWidgetMaker.hpp>

namespace gui
{
    ///  @brief AlarmClock vibration and volume settings
    ///
    class AlarmClockWindow : public BaseSettingsWindow
    {
      public:
        ///  Constructor
        ///  @app parent application pointer
        /// @audioModel audio settings model
        explicit AlarmClockWindow(app::ApplicationCommon *app,
                                  std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel);

      private:
        ///  Switches alarm vibration state
        void switchVibrationState();

        ///  Switches manual/system volume state
        void switchManualVolumeState();

        ///  Sets the volume
        ///  @param vol volume value
        void setVolume(uint8_t vol);

        ///  Build the list of options to be drawn
        ///  @return list of options
        std::list<Option> buildOptionsList() override;

        ///  Widget maker object
        OptionsWidgetMaker mWidgetMaker;

        ///  Flag describing if phone vibration is enabled
        bool mVibrationsEnabled = false;

        ///  Flag describing if manual volume is enabled
        bool mManualVolumeEnabled = false;

        /// Audio settings model
        std::unique_ptr<audio_settings::AbstractAudioSettingsModel> mAudioModel;

        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
