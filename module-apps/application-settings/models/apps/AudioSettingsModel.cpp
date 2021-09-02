// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioSettingsModel.hpp"

#include <service-audio/AudioServiceAPI.hpp>

namespace audio_settings
{

    constexpr audio::PlaybackType playbackTypeToAudioPlaybackType(PlaybackType type)
    {
        switch (type) {
        case PlaybackType::Notifications:
            return audio::PlaybackType::Notifications;
        case PlaybackType::KeypadSound:
            return audio::PlaybackType::KeypadSound;
        case PlaybackType::CallRingtone:
            return audio::PlaybackType::CallRingtone;
        case PlaybackType::TextMessageRingtone:
            return audio::PlaybackType::TextMessageRingtone;
        case PlaybackType::Alarm:
            return audio::PlaybackType::Alarm;
        }
        return audio::PlaybackType::None;
    }

    AudioSettingsModel::AudioSettingsModel(app::Application *application, PlaybackType playbackType)
        : application(application), playbackType(playbackTypeToAudioPlaybackType(playbackType))
    {}

    bool AudioSettingsModel::isVibrationEnabled()
    {
        auto settingState = AudioServiceAPI::GetVibrationSetting(application, playbackType);
        if (!settingState) {
            LOG_ERROR("Cannot read vibration setting for %s", audio::str(playbackType).c_str());
        }
        return (settingState.value_or(audio::SettingState::Disabled) == audio::SettingState::Enabled) ? true : false;
    }

    void AudioSettingsModel::setVibrationEnabled()
    {
        AudioServiceAPI::SetVibrationSetting(application, audio::SettingState::Enabled, playbackType);
    }

    void AudioSettingsModel::setVibrationDisabled()
    {
        AudioServiceAPI::SetVibrationSetting(application, audio::SettingState::Disabled, playbackType);
    }

    bool AudioSettingsModel::isSoundEnabled()
    {
        auto settingState = AudioServiceAPI::GetSoundSetting(application, playbackType);
        if (!settingState) {
            LOG_ERROR("Cannot read sound enabled setting for %s", audio::str(playbackType).c_str());
        }
        return (settingState.value_or(audio::SettingState::Disabled) == audio::SettingState::Enabled) ? true : false;
    }

    void AudioSettingsModel::setSoundEnabled()
    {
        AudioServiceAPI::SetSoundSetting(application, audio::SettingState::Enabled, playbackType);
    }

    void AudioSettingsModel::setSoundDisabled()
    {
        AudioServiceAPI::SetSoundSetting(application, audio::SettingState::Disabled, playbackType);
    }
    std::string AudioSettingsModel::getSound()
    {
        return AudioServiceAPI::GetSound(application, playbackType);
    }

    void AudioSettingsModel::setSound(std::string filePath)
    {
        AudioServiceAPI::SetSound(application, filePath, playbackType);
    }

    audio::Volume AudioSettingsModel::getVolume()
    {
        auto volume = AudioServiceAPI::GetVolume(application, playbackType);
        if (!volume) {
            LOG_ERROR("Cannot read volume level for %s", audio::str(playbackType).c_str());
        }
        return volume.value_or(0);
    }

    void AudioSettingsModel::setVolume(audio::Volume volume)
    {
        AudioServiceAPI::SetVolume(application, volume, playbackType);
    }

    audio::PlaybackType AudioSettingsModel::getPlaybackType()
    {
        return playbackType;
    }
} // namespace audio_settings
