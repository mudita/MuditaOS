// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/apps/AudioSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/windows/OptionsWidgetMaker.hpp>

namespace gui
{
    ///  @brief Phone (call) vibration and sound settings window
    class PhoneWindow : public BaseSettingsWindow
    {
      public:
        ///  Constructor
        ///  @app parent application pointer
        ///  @audioModel audio settings model
        explicit PhoneWindow(app::Application *app,
                             std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel);

      private:
        ///  Switches phone vibration state
        void switchVibrationState();

        ///  Switches phone sound state
        void switchSoundState();

        ///  Opens call ringtone selection window
        void openRingtoneWindow();

        ///  Build the list of options to be drawn
        ///  @return list of options
        std::list<Option> buildOptionsList() override;

        ///  Widget maker object
        OptionsWidgetMaker mWidgetMaker;

        ///  Flag describing if phone vibration is enabled
        bool mVibrationsEnabled = false;

        ///  Flag describing if phone sound is enabled
        bool mSoundEnabled = true;

        /// Audio settings model
        std::unique_ptr<audio_settings::AbstractAudioSettingsModel> mAudioModel;
    };
} // namespace gui
