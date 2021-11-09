// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAudioModel.hpp"
#include <apps-common/AsyncTask.hpp>

namespace app
{
    class AudioModel : public AbstractAudioModel, public app::AsyncCallbackReceiver
    {
      public:
        explicit AudioModel(ApplicationCommon *app);

        void setVolume(Volume volume, PlaybackType playbackType, OnStateChangeCallback &&callback) override;
        std::optional<Volume> getVolume(PlaybackType playbackType) override;
        void getVolume(PlaybackType playbackType, OnGetValueCallback &&callback) override;
        void play(const std::string &filePath, PlaybackType type, OnStateChangeCallback &&callback) override;
        void stop(OnStateChangeCallback &&callback) override;
        void pause(OnStateChangeCallback &&callback) override;
        void resume(OnStateChangeCallback &&callback) override;

      private:
        ApplicationCommon *app{};
    };
} // namespace app
