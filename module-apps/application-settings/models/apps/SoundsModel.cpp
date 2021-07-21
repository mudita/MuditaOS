// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsModel.hpp"

#include <application-settings/widgets/apps/SettingsSoundItem.hpp>

#include <ListView.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>

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
            item->activatedCallback = [=](gui::Item &) {
                auto fileRelativePath = sound.lexically_relative(purefs::dir::getCurrentOSPath());
                LOG_INFO("Setting sound to %s", fileRelativePath.c_str());
                model->setSound(fileRelativePath);
                app->returnToPreviousWindow();
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
