// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmOptionsItem.hpp"

#include <application-alarm-clock/data/AlarmsData.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/AudioOperations.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

namespace gui
{
    class AlarmMusicOptionsItem : public AlarmOptionsItem
    {
      private:
        enum class MusicStatus
        {
            Stop,
            Play
        };

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        /// pointer to audio operations which allows to make audio preview
        std::unique_ptr<app::AbstractAudioOperations> audioOperations;
        std::vector<tags::fetcher::Tags> alarmSoundList;
        MusicStatus musicStatus = MusicStatus::Stop;
        audio::Token currentlyPreviewedToken;
        std::string currentlyPreviewedPath;

        std::vector<tags::fetcher::Tags> getMusicFilesList();

      public:
        explicit AlarmMusicOptionsItem(app::ApplicationCommon *app,
                                       const std::string &description,
                                       std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                                       std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);

      private:
        [[nodiscard]] std::string getTitle(const std::string &filePath);
        [[nodiscard]] std::string getFilePath(const std::string &title);
        bool playAudioPreview(const std::string &path);
        bool pauseAudioPreview();
        bool resumeAudioPreview();
        bool stopAudioPreview();
    };

} /* namespace gui */