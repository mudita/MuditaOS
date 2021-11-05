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
#include <module-db/queries/multimedia_files/QueryMultimediaFilesGet.hpp>

#include <filesystem>

namespace constants
{
    inline constexpr auto cacheMaxSize{30};
    inline constexpr auto cacheThreshold{10};
} // namespace constants

namespace app::music
{
    ServiceAudioTagsFetcher::ServiceAudioTagsFetcher(ApplicationCommon *application) : application(application)
    {}

    std::optional<tags::fetcher::Tags> ServiceAudioTagsFetcher::getFileTags(const std::string &filePath) const
    {
        return tags::fetcher::fetchTags(filePath);
    }

    SongsRepository::SongsRepository(ApplicationCommon *application,
                                     std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                     std::string pathPrefix)
        : app::AsyncCallbackReceiver{application}, application{application},
          tagsFetcher(std::move(tagsFetcher)), pathPrefix{pathPrefix}
    {}

    void SongsRepository::initCache()
    {
        auto query = std::make_unique<db::multimedia_files::query::GetLimited>(0, constants::cacheMaxSize);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
        musicFilesModelCache.recordsOffset = 0;

        task->setCallback([this](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);

            if (result == nullptr) {
                return false;
            }

            return initCacheCallback(result->getResult(), result->getCount());
        });
        task->execute(application, this);
    }

    void SongsRepository::getMusicFilesList(std::uint32_t offset,
                                            std::uint32_t limit,
                                            const OnGetMusicFilesListCallback &callback)
    {
        auto query = std::make_unique<db::multimedia_files::query::GetLimitedByPath>(pathPrefix, offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);

        task->setCallback([this, callback, offset](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);
            musicFilesViewCache.records.clear();

            if (result == nullptr) {
                return false;
            }

            for (auto &record : result->getResult()) {
                musicFilesViewCache.records.push_back(record);
            }
            musicFilesViewCache.recordsOffset = offset;
            musicFilesViewCache.recordsCount  = result->getCount();

            if (callback) {
                callback(musicFilesViewCache.records, musicFilesViewCache.recordsCount);
            }
            return true;
        });
        task->execute(application, this);
    }

    std::size_t SongsRepository::getCachedFileIndex(const std::string &filePath) const
    {
        auto it = std::find_if(musicFilesModelCache.records.begin(),
                               musicFilesModelCache.records.end(),
                               [filePath](const auto &musicFile) { return musicFile.fileInfo.path == filePath; });

        if (it != musicFilesModelCache.records.end()) {
            return std::distance(musicFilesModelCache.records.begin(), it);
        }

        return std::numeric_limits<size_t>::max();
    }

    std::string SongsRepository::getNextFilePath(const std::string &filePath) const
    {
        const auto currentIndex = getCachedFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max() ||
            currentIndex >= musicFilesModelCache.records.size() - 1) {
            return "";
        }
        return musicFilesModelCache.records[currentIndex + 1].fileInfo.path;
    }

    std::string SongsRepository::getPreviousFilePath(const std::string &filePath) const
    {
        const auto currentIndex = getCachedFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max() || currentIndex == 0 ||
            currentIndex > musicFilesModelCache.records.size()) {
            return "";
        }
        return musicFilesModelCache.records[currentIndex - 1].fileInfo.path;
    }

    std::uint32_t SongsRepository::calculateOffset()
    {
        if (musicFilesViewCache.recordsCount < constants::cacheMaxSize) {
            return 0;
        }
        else if (musicFilesViewCache.recordsOffset < constants::cacheThreshold) {
            return 0;
        }
        else if (musicFilesViewCache.recordsOffset >
                 musicFilesViewCache.recordsCount - (constants::cacheMaxSize - constants::cacheThreshold)) {
            return musicFilesViewCache.recordsCount - constants::cacheMaxSize;
        }
        else {
            return musicFilesViewCache.recordsOffset - constants::cacheThreshold;
        }
    }

    void SongsRepository::updateRepository(const std::string &filePath)
    {
        const auto currentIndex = getCachedFileIndex(filePath);

        if (currentIndex == std::numeric_limits<size_t>::max()) {
            updateMusicFilesList(
                calculateOffset(),
                constants::cacheMaxSize,
                [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                       unsigned int repoRecordsCount,
                       std::uint32_t offset) { return newDataCallback(records, repoRecordsCount, offset); });
        }
        else if (currentIndex > musicFilesModelCache.records.size() - constants::cacheThreshold) {
            updateMusicFilesList(
                musicFilesModelCache.recordsOffset + musicFilesModelCache.records.size() + 1,
                constants::cacheThreshold,
                [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                       unsigned int repoRecordsCount,
                       std::uint32_t offset) { return newBackDataCallback(records, repoRecordsCount, offset); });
        }
        else if (currentIndex < constants::cacheThreshold) {
            updateMusicFilesList(
                musicFilesModelCache.recordsOffset > constants::cacheThreshold
                    ? musicFilesModelCache.recordsOffset - constants::cacheThreshold
                    : 0,
                musicFilesModelCache.recordsOffset > constants::cacheThreshold ? constants::cacheThreshold
                                                                               : musicFilesModelCache.recordsOffset,
                [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                       unsigned int repoRecordsCount,
                       std::uint32_t offset) { return newFrontDataCallback(records, repoRecordsCount, offset); });
        }
    }

    void SongsRepository::updateMusicFilesList(std::uint32_t offset,
                                               std::uint32_t limit,
                                               const OnUpdateMusicFilesListCallback &callback)
    {
        auto query = std::make_unique<db::multimedia_files::query::GetLimited>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);

        task->setCallback([offset, callback](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);

            if (result == nullptr) {
                return false;
            }

            return callback(result->getResult(), result->getCount(), offset);
        });
        task->execute(application, this);
    }

    bool SongsRepository::initCacheCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                            unsigned int repoRecordsCount)
    {
        musicFilesModelCache.records.clear();
        for (auto &record : records) {
            musicFilesModelCache.records.push_back(record);
        }
        musicFilesModelCache.recordsOffset = 0;
        musicFilesModelCache.recordsCount  = repoRecordsCount;

        return true;
    }

    bool SongsRepository::newDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                          unsigned int repoRecordsCount,
                                          std::uint32_t offset)
    {
        musicFilesModelCache.records.clear();
        for (auto &record : records) {
            musicFilesModelCache.records.push_back(record);
        }
        musicFilesModelCache.recordsOffset = offset;
        musicFilesModelCache.recordsCount  = repoRecordsCount;

        return true;
    }

    bool SongsRepository::newBackDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                              unsigned int repoRecordsCount,
                                              std::uint32_t offset)
    {
        for (auto &record : records) {
            musicFilesModelCache.records.push_back(record);
            musicFilesModelCache.records.erase(musicFilesModelCache.records.begin(),
                                               musicFilesModelCache.records.begin() + 1);
            musicFilesModelCache.recordsOffset++;
        }
        musicFilesModelCache.recordsCount = repoRecordsCount;

        return true;
    }

    bool SongsRepository::newFrontDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                               unsigned int repoRecordsCount,
                                               std::uint32_t offset)
    {
        auto iterator = 0;
        for (auto &record : records) {
            musicFilesModelCache.records.insert(musicFilesModelCache.records.begin() + iterator, record);
            musicFilesModelCache.records.pop_back();
            iterator++;
        }
        musicFilesModelCache.recordsOffset = offset;
        musicFilesModelCache.recordsCount  = repoRecordsCount;

        return true;
    }

    std::optional<db::multimedia_files::MultimediaFilesRecord> SongsRepository::getRecord(
        const std::string &filePath) const
    {
        const auto index = getCachedFileIndex(filePath);
        if (index == std::numeric_limits<size_t>::max()) {
            return std::nullopt;
        }
        return musicFilesModelCache.records[index];
    }

} // namespace app::music
