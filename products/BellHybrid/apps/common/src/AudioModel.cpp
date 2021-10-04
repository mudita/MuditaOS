// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/AudioModel.hpp"
#include <service-audio/AudioServiceAPI.hpp>

namespace
{
    constexpr audio::PlaybackType convertPlaybackType(app::AbstractAudioModel::PlaybackType type)
    {
        using Type = app::AbstractAudioModel::PlaybackType;
        switch (type) {
        case Type::Alarm:
            return audio::PlaybackType::Alarm;
        case Type::Chime:
            return audio::PlaybackType::Alarm;
        case Type::PreWakeup:
            return audio::PlaybackType::Alarm;
        case Type::Bedtime:
            return audio::PlaybackType::Alarm;
        default:
            return audio::PlaybackType::Alarm;
        }
    }
} // namespace

namespace app
{

    AudioModel::AudioModel(ApplicationCommon *app) : app{app}, asyncAudioOperations{app}
    {}
    void AudioModel::setVolume(AbstractAudioModel::Volume volume, PlaybackType playbackType)
    {
        AudioServiceAPI::SetVolume(app, volume, convertPlaybackType(playbackType));
    }
    bool AudioModel::play(const std::string &filePath,
                          const AbstractAudioOperations::OnPlayCallback &callback,
                          PlaybackType type)
    {
        return asyncAudioOperations.play(filePath, callback, convertPlaybackType(type));
    }
    bool AudioModel::pause(const audio::Token &token, const AbstractAudioOperations::OnPauseCallback &callback)
    {
        return asyncAudioOperations.pause(token, callback);
    }
    bool AudioModel::resume(const audio::Token &token, const AbstractAudioOperations::OnResumeCallback &callback)
    {
        return asyncAudioOperations.resume(token, callback);
    }
    bool AudioModel::stop(const audio::Token &token, const AbstractAudioOperations::OnStopCallback &callback)
    {
        return asyncAudioOperations.stop(token, callback);
    }
} // namespace app
