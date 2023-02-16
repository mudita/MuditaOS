﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsModel.hpp"
#include "AppWindow.hpp"

#include <application-settings/widgets/apps/SettingsSoundItem.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

#include <ListView.hpp>
#include <purefs/filesystem_paths.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

SoundsModel::SoundsModel(std::shared_ptr<AbstractSoundsPlayer> soundsPlayer) : soundsPlayer{std::move(soundsPlayer)}
{}

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

void SoundsModel::createData(app::ApplicationCommon *app, audio_settings::AbstractAudioSettingsModel *model)
{
    assert(model);
    assert(app);

    // configure according to type
    const auto folder = getSoundPath(model);

    // iterate through selected folder and collect all sounds names
    std::vector<std::filesystem::path> sounds;
    if (std::filesystem::is_directory(folder)) {
        LOG_INFO("Scanning sound folder: %s", folder.c_str());
        for (const auto &entry : std::filesystem::directory_iterator(folder)) {
            if (std::filesystem::is_directory(entry)) {
                continue;
            }

            const auto &filePath = entry.path();
            if (filePath.extension() == ".mp3") {
                sounds.push_back(filePath);
            }
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
    list->reset();
    eraseInternalData();
}

std::filesystem::path SoundsModel::getSoundPath(audio_settings::AbstractAudioSettingsModel *model)
{
    assert(model);
    switch (model->getPlaybackType()) {
    case audio::PlaybackType::CallRingtone:
        return purefs::dir::getAssetsDirPath() / "audio/ringtone";

    case audio::PlaybackType::TextMessageRingtone:
        return purefs::dir::getAssetsDirPath() / "audio/sms";

    case audio::PlaybackType::Notifications:
        return purefs::dir::getAssetsDirPath() / "audio/alarm";

    default:
        return purefs::dir::getAssetsDirPath() / "audio";
    }
}

void SoundsModel::applyItems(const std::vector<std::filesystem::path> &sounds,
                             app::ApplicationCommon *app,
                             audio_settings::AbstractAudioSettingsModel *model)
{
    auto currentItemIndex  = 0;
    auto selectedItemIndex = 0;

    const auto selectedSound = purefs::dir::getSystemDiskPath() / model->getSound();
    for (const auto &sound : sounds) {

        bool isSelected = false;
        if (sound == selectedSound) {
            isSelected        = true;
            selectedItemIndex = currentItemIndex;
        }

        std::string itemTitle;
        auto fileTags = tags::fetcher::fetchTags(sound);
        itemTitle     = fileTags.title;

        if (itemTitle.empty()) {
            itemTitle = sound.filename();
        }

        auto item = new gui::SettingsSoundItem(itemTitle, isSelected);

        switch (model->getPlaybackType()) {

        case audio::PlaybackType::CallRingtone:
        case audio::PlaybackType::TextMessageRingtone:
        case audio::PlaybackType::Notifications:
            item->activatedCallback = [=](gui::Item &) {
                const auto fileRelativePath = sound.lexically_relative(purefs::dir::getSystemDiskPath());
                LOG_INFO("Setting sound to %s", fileRelativePath.c_str());
                model->setSound(fileRelativePath);
                soundsPlayer->stop();
                app->returnToPreviousWindow();
                return true;
            };

            // callback to handle preview of the sound
            item->inputCallback = [=](gui::Item &item, const gui::InputEvent &event) {
                const auto fileRelativePath = sound.lexically_relative(purefs::dir::getSystemDiskPath());

                if (event.isShortRelease(gui::KeyCode::KEY_RF)) {
                    soundsPlayer->stop();
                    return false;
                }

                if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                    const auto window = app->getCurrentWindow();

                    if (!soundsPlayer->previouslyPlayed(fileRelativePath) ||
                        soundsPlayer->isInState(AbstractSoundsPlayer::State::Stopped)) {
                        window->navBarTemporaryMode(
                            utils::translate(style::strings::common::pause), gui::nav_bar::Side::Left, false);
                        return soundsPlayer->play(fileRelativePath, [=]() {
                            window->navBarTemporaryMode(
                                utils::translate(style::strings::common::play), gui::nav_bar::Side::Left, false);
                        });
                    }

                    if (soundsPlayer->isInState(AbstractSoundsPlayer::State::Playing)) {
                        window->navBarTemporaryMode(
                            utils::translate(style::strings::common::play), gui::nav_bar::Side::Left, false);
                        return soundsPlayer->pause();
                    }

                    if (soundsPlayer->isInState(AbstractSoundsPlayer::State::Paused)) {
                        window->navBarTemporaryMode(
                            utils::translate(style::strings::common::pause), gui::nav_bar::Side::Left, false);
                        return soundsPlayer->resume();
                    }
                }

                return false;
            };

            item->focusChangedCallback = [=](gui::Item &item) {
                const auto window = app->getCurrentWindow();
                if (!item.focus) {
                    window->navBarRestoreFromTemporaryMode();
                    return true;
                }

                const auto fileRelativePath = sound.lexically_relative(purefs::dir::getSystemDiskPath());
                if (!soundsPlayer->previouslyPlayed(fileRelativePath) &&
                    soundsPlayer->isInState(AbstractSoundsPlayer::State::Playing)) {
                    window->navBarTemporaryMode(
                        utils::translate(style::strings::common::pause), gui::nav_bar::Side::Left, false);
                    return soundsPlayer->play(fileRelativePath, [=]() {
                        window->navBarTemporaryMode(
                            utils::translate(style::strings::common::play), gui::nav_bar::Side::Left, false);
                    });
                }

                window->navBarTemporaryMode(
                    utils::translate(style::strings::common::play), gui::nav_bar::Side::Left, false);
                return true;
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
