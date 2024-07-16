// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPlayer.hpp"
#include <audio/AudioMessage.hpp>

namespace app::relaxation
{
    AbstractRelaxationPlayer::PlaybackMode RelaxationPlayer::getCurrentMode() const noexcept
    {
        return playbackMode;
    }

    RelaxationPlayer::RelaxationPlayer(AbstractRelaxationFadeModel &fadeModel, AbstractAudioModel &audioModel)
        : fadeModel{fadeModel}, audioModel{audioModel}
    {}

    void RelaxationPlayer::start(const std::string &filePath,
                                 AbstractRelaxationPlayer::PlaybackMode mode,
                                 AbstractAudioModel::OnStateChangeCallback &&stateChangeCallback,
                                 AbstractAudioModel::OnPlaybackFinishedCallback &&finishedCallback,
                                 std::optional<std::chrono::seconds> playbackDuration)
    {
        using Status = AbstractAudioModel::PlaybackFinishStatus;
        using Type   = AbstractAudioModel::PlaybackType;

        recentFilePath = filePath;
        playbackMode   = mode;

        auto onPlayerFinished = [callback = finishedCallback, this](Status status) {
            if (status == Status::Error) {
                callback(status); // First playback finished with error
            }
            else if (playbackMode == PlaybackMode::Looped) {
                audioModel.play(recentFilePath, Type::Multimedia, [&](audio::RetCode retCode) { // Replay in loop mode
                    if (retCode != audio::RetCode::Success) {
                        callback(Status::Error); // Replay fail in looped mode
                    }
                });
            }
            else {
                callback(Status::Normal); // Normal finish in single shot mode
            }
        };

        auto fadeParams = audio::FadeParams{.mode = getFadeMode(), .playbackDuration = playbackDuration};
        audioModel.setPlaybackFinishedCb(std::move(onPlayerFinished));
        audioModel.play(filePath, Type::Multimedia, std::move(stateChangeCallback), std::move(fadeParams));
    }

    audio::Fade RelaxationPlayer::getFadeMode() const
    {
        return fadeModel.getFade().getValue() ? audio::Fade::InOut : audio::Fade::Disable;
    }

    void RelaxationPlayer::stop(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = false;
        audioModel.stopPlayedByThis(std::move(callback));
    }

    void RelaxationPlayer::pause(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = true;
        audioModel.pause(std::move(callback));
    }

    void RelaxationPlayer::resume(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = false;
        audioModel.resume(std::move(callback));
    }

    bool RelaxationPlayer::isPaused()
    {
        return paused;
    }
} // namespace app::relaxation
