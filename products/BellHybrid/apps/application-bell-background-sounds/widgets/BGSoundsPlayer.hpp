// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-audio/Audio/AudioCommon.hpp>
#include <AsyncTask.hpp>

namespace app
{
    class ApplicationCommon;
}
class AudioEOFNotification;
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

        virtual ~AbstractBGSoundsPlayer() = default;

        using OnStateChangeCallback = std::function<void(audio::RetCode retCode)>;
        virtual void start(const std::string &filePath, PlaybackMode mode, OnStateChangeCallback callback) = 0;
        virtual void stop(OnStateChangeCallback callback)                                                  = 0;
        virtual void pause(OnStateChangeCallback callback)                                                 = 0;
        virtual void resume(OnStateChangeCallback callback)                                                = 0;
        virtual PlaybackMode getCurrentMode() const noexcept                                               = 0;
    };

    class BGSoundsPlayer : public AbstractBGSoundsPlayer, public app::AsyncCallbackReceiver
    {
        app::ApplicationCommon *app{};
        audio::Token token;
        std::string recentFilePath;
        PlaybackMode playbackMode = PlaybackMode::SingleShot;

        void start(const std::string &filePath, PlaybackMode mode, OnStateChangeCallback callback) override;
        void stop(OnStateChangeCallback callback) override;
        void pause(OnStateChangeCallback callback) override;
        void resume(OnStateChangeCallback callback) override;
        PlaybackMode getCurrentMode() const noexcept override;

      public:
        explicit BGSoundsPlayer(app::ApplicationCommon *app);

        auto handle(AudioEOFNotification *msg) -> std::shared_ptr<sys::Message>;
    };
} // namespace app::bgSounds
