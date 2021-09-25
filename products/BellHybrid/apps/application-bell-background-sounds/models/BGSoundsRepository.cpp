// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsRepository.hpp"

#include <algorithm>
#include <log/log.hpp>
#include <time/ScopedTime.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

#include <filesystem>

namespace app::bgSounds
{
    BGSoundsTagsFetcher::BGSoundsTagsFetcher(ApplicationCommon *application) : application(application)
    {}

    std::optional<tags::fetcher::Tags> BGSoundsTagsFetcher::getFileTags(const std::string &filePath) const
    {
        return tags::fetcher::fetchTags(filePath);
    }

    BGSoundsRepository::BGSoundsRepository(std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                           std::string musicFolderName)
        : tagsFetcher(std::move(tagsFetcher)), musicFolderName(std::move(musicFolderName))
    {}

    void BGSoundsRepository::scanMusicFilesList()
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

    std::vector<tags::fetcher::Tags> BGSoundsRepository::getMusicFilesList() const
    {
        return musicFiles;
    }

} // namespace app::bgSounds
