// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmMusicOptionsItem.hpp"

#include <service-audio/AudioServiceAPI.hpp>
#include <purefs/filesystem_paths.hpp>

namespace gui
{
    AlarmMusicOptionsItem::AlarmMusicOptionsItem(app::ApplicationCommon *app,
                                                 const std::string &description,
                                                 std::shared_ptr<SoundsPlayer> player,
                                                 std::function<void(const UTF8 &text)> navBarTemporaryMode,
                                                 std::function<void()> navBarRestoreFromTemporaryMode)
        : AlarmOptionsItem(description), navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {
        assert(app != nullptr);

        alarmSoundList = getMusicFilesList();
        std::vector<UTF8> printOptions;
        for (const auto &musicFile : getMusicFilesList()) {
            printOptions.push_back(musicFile.title);
        }
        optionSpinner->setData({printOptions});

        inputCallback = [=](gui::Item &item, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                if (!player->previouslyPlayed(getFilePath(optionSpinner->getCurrentValue())) ||
                    player->isInState(SoundsPlayer::State::Stopped)) {
                    player->play(getFilePath(optionSpinner->getCurrentValue()),
                                 [=]() { this->navBarTemporaryMode(utils::translate(style::strings::common::play)); });
                    this->navBarTemporaryMode(utils::translate(style::strings::common::pause));
                }
                else if (player->isInState(SoundsPlayer::State::Paused)) {
                    player->resume();
                    this->navBarTemporaryMode(utils::translate(style::strings::common::pause));
                }
                else {
                    player->pause();
                    this->navBarTemporaryMode(utils::translate(style::strings::common::play));
                }
            }

            // stop preview playback when we go back
            if (player->isInState(SoundsPlayer::State::Playing) && event.isShortRelease(gui::KeyCode::KEY_RF)) {
                player->stop();
            }
            auto res = optionSpinner->onInput(event);

            if (res && player->previouslyPlayed(getFilePath(optionSpinner->getCurrentValue())) &&
                player->isInState(SoundsPlayer::State::Playing)) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::pause));
            }
            else if (res) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::play));
            }

            return res;
        };

        focusChangedCallback = [=](Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);

            if (focus) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::play));
            }
            else {
                this->navBarRestoreFromTemporaryMode();
            }

            // stop preview playback when we loose focus
            if (!player->isInState(SoundsPlayer::State::Stopped)) {
                player->stop();
            }

            return true;
        };

        onSaveCallback = [=](std::shared_ptr<AlarmEventRecord> alarm) {
            // stop preview playback if it is played
            if (!player->isInState(SoundsPlayer::State::Stopped)) {
                player->stop();
            }
            alarm->musicTone = getFilePath(optionSpinner->getCurrentValue());
        };

        onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            optionSpinner->setCurrentValue(getTitle(alarm->musicTone));
        };
    }

    std::vector<tags::fetcher::Tags> AlarmMusicOptionsItem::getMusicFilesList()
    {
        const auto musicFolder = (purefs::dir::getAssetsPath() / "audio/alarm").string();
        std::vector<tags::fetcher::Tags> musicFiles;
        LOG_INFO("Scanning music folder: %s", musicFolder.c_str());
        for (const auto &ent : std::filesystem::directory_iterator(musicFolder)) {
            if (!ent.is_directory()) {
                const auto filePath = std::string(musicFolder) + "/" + ent.path().filename().c_str();
                auto fileTags       = tags::fetcher::fetchTags(filePath);
                musicFiles.push_back(fileTags);
                LOG_DEBUG("file: %s found", ent.path().filename().c_str());
            }
        }
        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
        return musicFiles;
    }

    std::string AlarmMusicOptionsItem::getTitle(const std::string &filePath)
    {
        for (const auto &musicFile : alarmSoundList) {
            if (musicFile.filePath == filePath) {
                return musicFile.title;
            }
        }
        return std::string();
    }

    std::string AlarmMusicOptionsItem::getFilePath(const std::string &title)
    {
        for (const auto &musicFile : alarmSoundList) {
            if (musicFile.title == title) {
                return musicFile.filePath;
            }
        }
        return std::string();
    }
} /* namespace gui */
