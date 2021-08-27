// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsRepository.hpp"

#include <algorithm>
#include <log.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-audio/AudioServiceName.hpp>
#include <time/ScopedTime.hpp>
#include <service-audio/AudioMessage.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

#include <filesystem>

namespace app::music_player
{
    ServiceAudioTagsFetcher::ServiceAudioTagsFetcher(Application *application) : application(application)
    {}

    std::optional<tags::fetcher::Tags> ServiceAudioTagsFetcher::getFileTags(const std::string &filePath) const
    {
        return tags::fetcher::fetchTags(filePath);
    }

    SongsRepository::SongsRepository(std::unique_ptr<AbstractTagsFetcher> tagsFetcher, std::string musicFolderName)
        : tagsFetcher(std::move(tagsFetcher)), musicFolderName(std::move(musicFolderName))
    {}

    void SongsRepository::scanMusicFilesList()
    {
        musicFiles.clear();

        LOG_INFO("Scanning music folder: %s", musicFolderName.c_str());
        {
            auto time = utils::time::Scoped("fetch tags time");
            for (const auto &entry : std::filesystem::directory_iterator(musicFolderName)) {
                if (!std::filesystem::is_directory(entry)) {
                    const auto &filePath = entry.path();
                    const auto fileTags  = tagsFetcher->getFileTags(filePath);
                    if (fileTags) {
                        musicFiles.push_back(*fileTags);
                    }
                    else {
                        LOG_ERROR("Scanned not an audio file, skipped");
                    }
                }
            }
            std::sort(
                musicFiles.begin(), musicFiles.end(), [](const tags::fetcher::Tags &t1, const tags::fetcher::Tags &t2) {
                    return t1.filePath < t2.filePath;
                });
        }
        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
    }

    std::vector<tags::fetcher::Tags> SongsRepository::getMusicFilesList() const
    {
        return musicFiles;
    }

    std::size_t SongsRepository::getFileIndex(const std::string &filePath) const
    {
        auto it = std::find_if(musicFiles.begin(), musicFiles.end(), [filePath](const auto &musicFile) {
            return musicFile.filePath == filePath;
        });

        if (it != musicFiles.end()) {
            return std::distance(musicFiles.begin(), it);
        }

        return std::numeric_limits<size_t>::max();
    }

    std::string SongsRepository::getNextFilePath(const std::string &filePath) const
    {
        const auto currentIndex = getFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max() || currentIndex == musicFiles.size() - 1) {
            return "";
        }
        return musicFiles[currentIndex + 1].filePath;
    }

    std::string SongsRepository::getPreviousFilePath(const std::string &filePath) const
    {
        const auto currentIndex = getFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max() || currentIndex == 0) {
            return "";
        }
        return musicFiles[currentIndex - 1].filePath;
    }
} // namespace app::music_player
