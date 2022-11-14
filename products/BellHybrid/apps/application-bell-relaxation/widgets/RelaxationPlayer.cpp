// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPlayer.hpp"
#include <audio/AudioMessage.hpp>

namespace app::relaxation
{
    AbstractRelaxationPlayer::PlaybackMode RelaxationPlayer::getCurrentMode() const noexcept
    {
        return playbackMode;
    }
    RelaxationPlayer::RelaxationPlayer(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {}
    void RelaxationPlayer::start(const std::string &filePath,
                                 AbstractRelaxationPlayer::PlaybackMode mode,
                                 AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        recentFilePath = filePath;
        playbackMode   = mode;
        audioModel.play(filePath, AbstractAudioModel::PlaybackType::Multimedia, std::move(callback));
        audioModel.setPlaybackFinishedCb([&]() {
            if (playbackMode == PlaybackMode::Looped) {
                audioModel.play(recentFilePath, AbstractAudioModel::PlaybackType::Multimedia, {});
            }
        });
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
