// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmOptionsItem.hpp"

#include <application-alarm-clock/data/AlarmsData.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/AudioOperations.hpp>
#include <apps-common/audio/SoundsPlayer.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

namespace gui
{
    class AlarmMusicOptionsItem : public AlarmOptionsItem
    {
      private:
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;

        /// pointer to audio operations which allows to make audio preview
        std::vector<tags::fetcher::Tags> alarmSoundList;
        std::vector<tags::fetcher::Tags> getMusicFilesList();

      public:
        explicit AlarmMusicOptionsItem(app::ApplicationCommon *app,
                                       const std::string &description,
                                       std::shared_ptr<SoundsPlayer> player,
                                       std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                                       std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);

      private:
        [[nodiscard]] std::string getTitle(const std::string &filePath);
        [[nodiscard]] std::string getFilePath(const std::string &title);
    };

} /* namespace gui */
