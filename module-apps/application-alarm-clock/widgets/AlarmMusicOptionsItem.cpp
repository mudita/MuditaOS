// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmMusicOptionsItem.hpp"

#include <service-audio/AudioServiceAPI.hpp>
#include <purefs/filesystem_paths.hpp>

namespace gui
{
    AlarmMusicOptionsItem::AlarmMusicOptionsItem(app::ApplicationCommon *app,
                                                 const std::string &description,
                                                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : AlarmOptionsItem(description), bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        assert(app != nullptr);

        // create audioOperations to allow sounds preview
        audioOperations = std::make_unique<app::AsyncAudioOperations>(app);

        alarmSoundList = getMusicFilesList();
        std::vector<UTF8> printOptions;
        for (const auto &musicFile : getMusicFilesList()) {
            printOptions.push_back(musicFile.title);
        }
        optionSpinner->setData({printOptions});

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                if (getFilePath(optionSpinner->getCurrentValue()) != currentlyPreviewedPath) {
                    playAudioPreview(getFilePath(optionSpinner->getCurrentValue()));
                }
                else if (musicStatus == MusicStatus::Stop) {
                    resumeAudioPreview();
                }
                else {
                    pauseAudioPreview();
                }
            }

            // stop preview playback when we go back
            if (musicStatus == MusicStatus::Play && event.isShortRelease(gui::KeyCode::KEY_RF)) {
                stopAudioPreview();
            }

            return optionSpinner->onInput(event);
        };

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);

            if (focus) {
                this->bottomBarTemporaryMode(utils::translate(style::strings::common::play_pause));
            }
            else {
                this->bottomBarRestoreFromTemporaryMode();
            }

            // stop preview playback when we loose focus
            if (musicStatus == MusicStatus::Play) {
                stopAudioPreview();
            }

            return true;
        };

        onSaveCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            // stop preview playback if it is played
            if (musicStatus == MusicStatus::Play) {
                stopAudioPreview();
            }

            alarm->musicTone = getFilePath(optionSpinner->getCurrentValue());
        };

        onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            optionSpinner->setCurrentValue(getTitle(alarm->musicTone));
        };
    }

    std::vector<tags::fetcher::Tags> AlarmMusicOptionsItem::getMusicFilesList()
    {
        const auto musicFolder = (purefs::dir::getCurrentOSPath() / "assets/audio/alarm").string();
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

    bool AlarmMusicOptionsItem::playAudioPreview(const std::string &path)
    {
        return audioOperations->play(path, [this, path](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus             = MusicStatus::Play;
            currentlyPreviewedToken = token;
            currentlyPreviewedPath  = path;
        });
    }

    bool AlarmMusicOptionsItem::pauseAudioPreview()
    {
        return audioOperations->pause(currentlyPreviewedToken, [this](audio::RetCode retCode, audio::Token token) {
            if (token != currentlyPreviewedToken) {
                LOG_ERROR("Audio preview pause failed: wrong token");
                return;
            }
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus = MusicStatus::Stop;
        });
    }

    bool AlarmMusicOptionsItem::resumeAudioPreview()
    {
        return audioOperations->resume(currentlyPreviewedToken, [this](audio::RetCode retCode, audio::Token token) {
            if (token != currentlyPreviewedToken) {
                LOG_ERROR("Audio preview resume failed: wrong token");
                return;
            }

            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus = MusicStatus::Play;
        });
    }

    bool AlarmMusicOptionsItem::stopAudioPreview()
    {
        if (currentlyPreviewedToken.IsValid()) {
            musicStatus            = MusicStatus::Stop;
            currentlyPreviewedPath = "";
            return audioOperations->stop(currentlyPreviewedToken, [](audio::RetCode, audio::Token) {});
        }
        return false;
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
