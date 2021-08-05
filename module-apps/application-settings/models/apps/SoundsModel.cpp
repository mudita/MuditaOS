// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsModel.hpp"

#include <application-settings/widgets/apps/SettingsSoundItem.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <ListView.hpp>

unsigned int SoundsModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int SoundsModel::getMinimalItemSpaceRequired() const
{
    return style::window::label::big_h + style::margins::big;
}

void SoundsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *SoundsModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void SoundsModel::createData(app::Application *app, audio_settings::AbstractAudioSettingsModel *model)
{
    assert(model);
    assert(app);

    // create audioOperations to allow shounds preview
    audioOperations = std::make_unique<app::AsyncAudioOperations>(app);

    // configure according to type
    std::filesystem::path folder = getSoundPath(model);

    // iterate through selected folder and collect all sounds names
    std::vector<std::filesystem::path> sounds;
    if (std::filesystem::is_directory(folder)) {
        LOG_INFO("Scanning sound folder: %s", folder.c_str());
        for (const auto &entry : std::filesystem::directory_iterator(folder)) {
            if (std::filesystem::is_directory(entry)) {
                continue;
            }

            const auto &filePath = entry.path();
            if (filePath.extension() == ".mp3")
                sounds.push_back(filePath);
        }
        LOG_INFO("Found %d sounds in folder %s", static_cast<int>(sounds.size()), folder.c_str());
    }
    else {
        LOG_ERROR("Cannot find directory: %s", folder.c_str());
    }

    applyItems(sounds, app, model);
}

void SoundsModel::clearData()
{
    list->clear();
    list->rebuildList();
}

std::filesystem::path SoundsModel::getSoundPath(audio_settings::AbstractAudioSettingsModel *model)
{
    assert(model);
    switch (model->getPlaybackType()) {
    case audio::PlaybackType::CallRingtone:
        return purefs::dir::getCurrentOSPath() / "assets/audio/ringtone";

    case audio::PlaybackType::TextMessageRingtone:
        return purefs::dir::getCurrentOSPath() / "assets/audio/sms";

    case audio::PlaybackType::Notifications:
        return purefs::dir::getCurrentOSPath() / "assets/audio/alarm";

    default:
        return purefs::dir::getCurrentOSPath() / "assets/audio";
    }
}

void SoundsModel::applyItems(const std::vector<std::filesystem::path> &sounds,
                             app::Application *app,
                             audio_settings::AbstractAudioSettingsModel *model)
{
    auto currentItemIndex  = 0;
    auto selectedItemIndex = 0;

    std::string selectedSound = purefs::dir::getCurrentOSPath() / model->getSound();
    for (const auto &sound : sounds) {

        bool isSelected = false;
        if (sound == selectedSound) {
            isSelected        = true;
            selectedItemIndex = currentItemIndex;
        }

        std::string itemTitle;
        auto fileTags = AudioServiceAPI::GetFileTags(app, sound);
        if (fileTags) {
            itemTitle = fileTags->title;
        }

        if (itemTitle.empty()) {
            itemTitle = sound.filename();
        }

        auto item = new gui::SettingsSoundItem(itemTitle, isSelected);

        switch (model->getPlaybackType()) {

        case audio::PlaybackType::CallRingtone:
        case audio::PlaybackType::TextMessageRingtone:
        case audio::PlaybackType::Notifications:
            // callback when user selects the sound
            item->activatedCallback = [=](gui::Item &) {
                auto fileRelativePath = sound.lexically_relative(purefs::dir::getCurrentOSPath());
                LOG_INFO("Setting sound to %s", fileRelativePath.c_str());
                model->setSound(fileRelativePath);
                stopAudioPreview();
                app->returnToPreviousWindow();
                return true;
            };

            // callback to handle preview of the sound
            item->inputCallback = [=](gui::Item &item, const gui::InputEvent &event) {
                auto fileRelativePath = sound.lexically_relative(purefs::dir::getCurrentOSPath());

                if (event.isShortRelease(gui::KeyCode::KEY_RF)) {
                    stopAudioPreview();
                }

                else if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                    if (!currentlyPreviewedToken.IsValid() || fileRelativePath != currentlyPreviewedPath) {
                        return playAudioPreview(fileRelativePath);
                    }

                    else if (currentlyPreviewedToken.IsValid() && fileRelativePath == currentlyPreviewedPath &&
                             myPreviewState == PreviewState::Playing) {
                        return pauseAudioPreview();
                    }

                    else if (currentlyPreviewedToken.IsValid() && fileRelativePath == currentlyPreviewedPath &&
                             myPreviewState == PreviewState::NotPlaying) {
                        return resumeAudioPreview();
                    }
                }

                return false;
            };

            break;

        default:
            item->activatedCallback = [=](gui::Item &) {
                app->returnToPreviousWindow();
                return true;
            };
            break;
        }

        internalData.push_back(item);
        ++currentItemIndex;
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList(gui::listview::RebuildType::OnPageElement, selectedItemIndex);
}

bool SoundsModel::playAudioPreview(const std::string &path)
{
    return audioOperations->play(path, [this, path](audio::RetCode retCode, audio::Token token) {
        if (retCode != audio::RetCode::Success || !token.IsValid()) {
            LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                      str(retCode).c_str(),
                      token.IsValid());
            return;
        }
        myPreviewState          = PreviewState::Playing;
        currentlyPreviewedToken = token;
        currentlyPreviewedPath  = path;
    });
}

bool SoundsModel::pauseAudioPreview()
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
        myPreviewState = PreviewState::NotPlaying;
    });
}

bool SoundsModel::resumeAudioPreview()
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

        myPreviewState = PreviewState::Playing;
    });
}

bool SoundsModel::stopAudioPreview()
{
    if (currentlyPreviewedToken.IsValid()) {
        return audioOperations->stop(currentlyPreviewedToken, [](audio::RetCode, audio::Token) {
            // do nothing as we are about to be closed
        });
    }
    return false;
}
