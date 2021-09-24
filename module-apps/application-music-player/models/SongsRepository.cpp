// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsRepository.hpp"

#include <algorithm>
#include <log/log.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-audio/AudioServiceName.hpp>
#include <time/ScopedTime.hpp>
#include <service-audio/AudioMessage.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>

#include <filesystem>

namespace app::music_player
{
    ServiceAudioTagsFetcher::ServiceAudioTagsFetcher(ApplicationCommon *application) : application(application)
    {}

    std::optional<tags::fetcher::Tags> ServiceAudioTagsFetcher::getFileTags(const std::string &filePath) const
    {
        return tags::fetcher::fetchTags(filePath);
    }

    SongsRepository::SongsRepository(ApplicationCommon *application, std::unique_ptr<AbstractTagsFetcher> tagsFetcher)
        : app::AsyncCallbackReceiver{application}, application{application}, tagsFetcher(std::move(tagsFetcher))
    {}

    void SongsRepository::getMusicFilesList(std::uint32_t offset,
                                            std::uint32_t limit,
                                            const OnGetMusicFilesListCallback &callback)
    {
        auto query = std::make_unique<db::multimedia_files::query::GetLimited>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);

        task->setCallback([this, callback](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);
            musicFiles.clear();

            if (result == nullptr) {
                return false;
            }

            if (callback) {
                for (auto &record : result->getResult()) {
                    musicFiles.push_back(record);
                }
                callback(musicFiles, result->getCount());
            }
            return true;
        });
        task->execute(application, this);
    }

    std::size_t SongsRepository::getFileIndex(const std::string &filePath) const
    {
        auto it = std::find_if(musicFiles.begin(), musicFiles.end(), [filePath](const auto &musicFile) {
            return musicFile.fileInfo.path == filePath;
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
        return musicFiles[currentIndex + 1].fileInfo.path;
    }

    std::string SongsRepository::getPreviousFilePath(const std::string &filePath) const
    {
        const auto currentIndex = getFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max() || currentIndex == 0) {
            return "";
        }
        return musicFiles[currentIndex - 1].fileInfo.path;
    }

} // namespace app::music_player
