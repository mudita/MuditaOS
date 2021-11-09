// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace service
{
    class AudioEOFNotification;
}

namespace app::bgSounds
{
    class AbstractBGSoundsPlayer
    {
      public:
        enum class PlaybackMode
        {
            Looped,
            SingleShot
        };

        virtual ~AbstractBGSoundsPlayer()                                         = default;
        virtual void start(const std::string &filePath,
                           PlaybackMode mode,
                           AbstractAudioModel::OnStateChangeCallback &&callback)  = 0;
        virtual void stop(AbstractAudioModel::OnStateChangeCallback &&callback)   = 0;
        virtual void pause(AbstractAudioModel::OnStateChangeCallback &&callback)  = 0;
        virtual void resume(AbstractAudioModel::OnStateChangeCallback &&callback) = 0;
        virtual PlaybackMode getCurrentMode() const noexcept                      = 0;
        virtual bool isPaused()                                                   = 0;
    };

    class BGSoundsPlayer : public AbstractBGSoundsPlayer
    {
      public:
        explicit BGSoundsPlayer(AbstractAudioModel &audioModel);

        auto handle(service::AudioEOFNotification *msg) -> std::shared_ptr<sys::Message>;

      private:
        void start(const std::string &filePath,
                   PlaybackMode mode,
                   AbstractAudioModel::OnStateChangeCallback &&callback) override;
        void stop(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        void pause(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        void resume(AbstractAudioModel::OnStateChangeCallback &&callback) override;
        PlaybackMode getCurrentMode() const noexcept override;
        bool isPaused() override;

        AbstractAudioModel &audioModel;
        std::string recentFilePath;
        PlaybackMode playbackMode = PlaybackMode::SingleShot;
        bool paused{false};
    };
} // namespace app::bgSounds
