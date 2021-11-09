// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsPlayer.hpp"
#include <audio/AudioMessage.hpp>

namespace app::bgSounds
{
    auto BGSoundsPlayer::handle(service::AudioEOFNotification *msg) -> std::shared_ptr<sys::Message>
    {
        if (playbackMode == PlaybackMode::Looped) {
            audioModel.play(recentFilePath, AbstractAudioModel::PlaybackType::Multimedia, {});
        }
        return sys::msgHandled();
    }
    AbstractBGSoundsPlayer::PlaybackMode BGSoundsPlayer::getCurrentMode() const noexcept
    {
        return playbackMode;
    }
    BGSoundsPlayer::BGSoundsPlayer(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {}
    void BGSoundsPlayer::start(const std::string &filePath,
                               AbstractBGSoundsPlayer::PlaybackMode mode,
                               AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        recentFilePath = filePath;
        playbackMode   = mode;
        audioModel.play(filePath, AbstractAudioModel::PlaybackType::Multimedia, std::move(callback));
    }
    void BGSoundsPlayer::stop(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = false;
        audioModel.stop(std::move(callback));
    }
    void BGSoundsPlayer::pause(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = true;
        audioModel.pause(std::move(callback));
    }
    void BGSoundsPlayer::resume(AbstractAudioModel::OnStateChangeCallback &&callback)
    {
        paused = false;
        audioModel.resume(std::move(callback));
    }
    bool BGSoundsPlayer::isPaused()
    {
        return paused;
    }
} // namespace app::bgSounds
