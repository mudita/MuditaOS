// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace audio
{
    enum class PlaybackType;
}

namespace audio_settings
{
    enum class PlaybackType
    {
        Notifications,
        KeypadSound,
        CallRingtone,
        TextMessageRingtone,
        Alarm
    };

    /// @brief Abstract audio settings model interface class
    class AbstractAudioSettingsModel
    {
      public:
        virtual ~AbstractAudioSettingsModel() noexcept = default;

        [[nodiscard]] virtual bool isVibrationEnabled() = 0;
        virtual void setVibrationEnabled()              = 0;
        virtual void setVibrationDisabled()             = 0;
        [[nodiscard]] virtual bool isSoundEnabled()     = 0;
        virtual void setSoundEnabled()                  = 0;
        virtual void setSoundDisabled()                 = 0;
        /// @return sound file path, returns empty string if not found
        [[nodiscard]] virtual std::string getSound() = 0;
        /// @param sound file path
        virtual void setSound(std::string filePath)                 = 0;
        [[nodiscard]] virtual audio::Volume getVolume()             = 0;
        virtual void setVolume(audio::Volume vol)                   = 0;
        [[nodiscard]] virtual audio::PlaybackType getPlaybackType() = 0;
    };

    class AudioSettingsModel : public AbstractAudioSettingsModel
    {
      public:
        /// @param application application pointer
        /// @param playbackType playback type
        AudioSettingsModel(app::ApplicationCommon *application, PlaybackType playbackType);

        bool isVibrationEnabled() override;
        void setVibrationEnabled() override;
        void setVibrationDisabled() override;
        bool isSoundEnabled() override;
        void setSoundEnabled() override;
        void setSoundDisabled() override;
        std::string getSound() override;
        void setSound(std::string) override;
        audio::Volume getVolume() override;
        void setVolume(audio::Volume vol) override;
        audio::PlaybackType getPlaybackType() override;

      private:
        app::ApplicationCommon *application = nullptr;
        audio::PlaybackType playbackType;
    };
} // namespace audio_settings
