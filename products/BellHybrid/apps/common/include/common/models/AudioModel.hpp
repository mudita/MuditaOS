// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAudioModel.hpp"
#include <apps-common/AudioOperations.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    class AudioModel : public AbstractAudioModel
    {
      public:
        explicit AudioModel(ApplicationCommon *app);

        void setVolume(Volume volume, PlaybackType playbackType) override;
        bool play(const std::string &filePath, const OnPlayCallback &callback, PlaybackType type) override;
        bool pause(const audio::Token &token, const OnPauseCallback &callback) override;
        bool resume(const audio::Token &token, const OnResumeCallback &callback) override;
        bool stop(const audio::Token &token, const OnStopCallback &callback) override;

      private:
        ApplicationCommon *app{};
        AsyncAudioOperations asyncAudioOperations;
    };
} // namespace app
