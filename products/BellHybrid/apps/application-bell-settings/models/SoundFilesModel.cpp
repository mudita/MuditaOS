// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundFilesModel.hpp"

#include "AlarmSoundPaths.hpp"

#include <log/log.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

#include <algorithm>

using namespace app::bell_settings;

AlarmSoundsModel::AlarmSoundsModel(AlarmSoundType type) : type{type}
{}

auto AlarmSoundsModel::getSoundFiles() const -> std::vector<std::filesystem::path>
{
    const auto dir = getSoundFilesDir(type);
    if (!std::filesystem::is_directory(dir)) {
        LOG_ERROR("Unable to find directory: %s", dir.c_str());
        return {};
    }
    return scanDirForFiles(dir, {".mp3"});
}

auto AlarmSoundsModel::getSoundFilesDir(AlarmSoundType type) -> std::filesystem::path
{
    switch (type) {
    case AlarmSoundType::PreWakeUp:
        return alarms::paths::getPreWakeUpChimesDir();
    }
    return {};
}

auto AlarmSoundsModel::scanDirForFiles(const std::filesystem::path &dir, std::vector<std::string> extensions)
    -> std::vector<std::filesystem::path>
{
    std::vector<std::filesystem::path> files;
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }

        const auto &filePath = entry.path();
        if (const auto it = std::find(extensions.begin(), extensions.end(), filePath.extension());
            it != extensions.end()) {
            files.push_back(filePath);
        }
    }
    return files;
}

auto AlarmSoundDelegate::fromModel(const AlarmSoundsModel &model) -> std::vector<AlarmSoundDelegate>
{
    const auto files = model.getSoundFiles();

    std::vector<AlarmSoundDelegate> delegates;
    delegates.reserve(files.size());
    std::transform(files.begin(), files.end(), std::back_inserter(delegates), [](const auto &filepath) {
        return AlarmSoundDelegate{filepath};
    });
    return delegates;
}

AlarmSoundDelegate::AlarmSoundDelegate(const std::filesystem::path &filePath) : path{filePath}
{}

AlarmSoundDelegate::AlarmSoundDelegate(AlarmSoundType type, const std::string &filename)
    : path{AlarmSoundsModel::getSoundFilesDir(type) / filename}
{}

AlarmSoundDelegate::operator UTF8() const
{
    auto tags = tags::fetcher::fetchTags(path);
    return tags.title;
}
