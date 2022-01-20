// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/apps/AudioSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/windows/OptionsWidgetMaker.hpp>

namespace gui
{
    ///  @brief Messages vibration, sound, and other settings window
    ///
    class MessagesWindow : public BaseSettingsWindow
    {
      public:
        ///  Constructor
        ///  @app parent application pointer
        ///  @audioModel audio settings model
        explicit MessagesWindow(app::ApplicationCommon *app,
                                std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel);

      private:
        ///  Switches messages vibration state
        void switchVibrationState();

        ///  Switches messages sound state
        void switchSoundState();

        ///  Opens message sound selection window
        void openMessageSoundWindow();

        ///  Switches option to show unread messages first
        void switchShowUnreadFirst();

        ///  Opens message templates window
        void openMessageTemplates();

        ///  Build the list of options to be drawn
        ///  @return list of options
        std::list<Option> buildOptionsList() override;

        ///  Widget maker object
        OptionsWidgetMaker mWidgetMaker;

        ///  Flag describing if messages vibration is enabled
        bool mVibrationsEnabled = false;

        ///  Flag describing if messages sound is enabled
        bool mSoundEnabled = true;

        ///  Flag describing if unread messages are shown first
        bool mShowUnreadMessagesFirst = true;

        /// Audio settings model
        std::unique_ptr<audio_settings::AbstractAudioSettingsModel> mAudioModel;

        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
