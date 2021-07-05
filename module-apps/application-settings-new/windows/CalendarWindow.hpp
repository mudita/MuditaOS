// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "OptionsWidgetMaker.hpp"
#include <application-settings-new/models/AudioSettingsModel.hpp>

namespace gui
{
    ///  @brief Calendar vibration and sound settings window
    ///
    class CalendarWindow : public BaseSettingsWindow
    {
      public:
        ///  Constructor
        ///  @app parent application pointer
        ///  @audioModel audio settings model
        explicit CalendarWindow(app::Application *app,
                                std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel);

      private:
        ///  Switches calendar vibration state
        void switchVibrationState();

        ///  Switches calendar sound state
        void switchSoundState();

        ///  Opens notification sound selection window
        void openNoticicationSoundWindow();

        ///  Build the list of options to be drawn
        ///  @return list of options
        std::list<Option> buildOptionsList() override;

        ///  Widget maker object
        OptionsWidgetMaker mWidgetMaker;

        ///  Flag describing if calendar vibration is enabled
        bool mVibrationsEnabled = false;

        ///  Flag describing if calendar sound is enabled
        bool mSoundEnabled = true;

        /// Audio settings model
        std::unique_ptr<audio_settings::AbstractAudioSettingsModel> mAudioModel;
    };
} // namespace gui
