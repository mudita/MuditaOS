// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>
#include <common/models/AbstractRelaxationFadeModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace service
{
    class AudioEOFNotification;
}

namespace app::relaxation
{
    class AbstractRelaxationPlayer
    {
      public:
        virtual ~AbstractRelaxationPlayer()                                                     = default;
        virtual void start(const std::string &filePath,
                           const AbstractAudioModel::PlaybackMode &mode,
                           AbstractAudioModel::OnStateChangeCallback &&callback,
                           AbstractAudioModel::OnPlaybackFinishedCallback &&finishedCallback,
                           std::optional<std::chrono::seconds> playbackDuration = std::nullopt) = 0;
        virtual void stop(AbstractAudioModel::OnStateChangeCallback &&callback)                 = 0;
        virtual void pause(AbstractAudioModel::OnStateChangeCallback &&callback)                = 0;
        virtual void resume(AbstractAudioModel::OnStateChangeCallback &&callback)               = 0;
        [[nodiscard]] virtual AbstractAudioModel::PlaybackMode getCurrentMode() const noexcept  = 0;
        [[nodiscard]] virtual audio::Fade getFadeMode() const                                   = 0;
        [[nodiscard]] virtual bool isPaused() const noexcept                                    = 0;
    };

    class RelaxationPlayer : public AbstractRelaxationPlayer
    {
      public:
        RelaxationPlayer(AbstractRelaxationFadeModel &fadeModel, AbstractAudioModel &audioModel);

      private:
        void start(const std::string &filePath,
                   const AbstractAudioModel::PlaybackMode &mode,
                   AbstractAudioModel::OnStateChangeCallback &&callback,
                   AbstractAudioModel::OnPlaybackFinishedCallback &&finishedCallback,
                   std::optional<std::chrono::seconds> playbackDuration = std::nullopt) override;
        void stop(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        void pause(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        void resume(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        [[nodiscard]] AbstractAudioModel::PlaybackMode getCurrentMode() const noexcept override;
        [[nodiscard]] audio::Fade getFadeMode() const override;
        [[nodiscard]] bool isPaused() const noexcept override;

        AbstractRelaxationFadeModel &fadeModel;
        AbstractAudioModel &audioModel;
        std::string recentFilePath;
        AbstractAudioModel::PlaybackMode playbackMode{AbstractAudioModel::PlaybackMode::Single};
        bool paused{false};
    };
} // namespace app::relaxation
