// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include "RelaxationSongsRepository.hpp"
#include <module-db/queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesCount.hpp>
#include <AsyncTask.hpp>
#include <vector>

namespace
{
    std::uint32_t calculateOffset(std::uint32_t offset, std::uint32_t filesCount)
    {
        if (offset >= filesCount) {
            return offset - filesCount;
        }
        return offset;
    }
    std::pair<std::uint32_t, std::uint32_t> calculateNewOffsetAndLimit(std::uint32_t offset,
                                                                       std::uint32_t limit,
                                                                       std::uint32_t loadedFiles)
    {
        const auto newLimit  = limit - loadedFiles;
        const auto newOffset = offset + loadedFiles;
        return {newOffset, newLimit};
    }
} // namespace
namespace app::relaxation
{

    RelaxationSongsRepository::RelaxationSongsRepository(ApplicationCommon *application,
                                                         const std::map<MusicType, std::string> &pathPrefixes)
        : app::AsyncCallbackReceiver{application}, application{application}, pathPrefixes{pathPrefixes}
    {}

    void RelaxationSongsRepository::init()
    {
        for (const auto &[musicType, musicPath] : pathPrefixes) {
            updateFilesCount(musicType, musicPath);
        }
    }

    void RelaxationSongsRepository::updateFilesCount(const MusicType &type, const std::string &path)
    {
        auto query = std::make_unique<db::multimedia_files::query::GetCountForPath>(path);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
        task->setCallback([this, &type](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(response);
            if (result == nullptr) {
                return false;
            }
            return updateCountCallback(type, result->getCount());
        });
        task->execute(application, this);
    }

    bool RelaxationSongsRepository::updateCountCallback(const MusicType &type, std::uint32_t count)
    {
        filesPerType[type] = count;
        return true;
    }

    void RelaxationSongsRepository::getMusicFiles(
        const MusicType &musicType,
        std::uint32_t offset,
        std::uint32_t limit,
        const RelaxationSongsRepository::OnGetMusicFilesListCallback &viewUpdateCallback)
    {
        auto taskCallback = [this, viewUpdateCallback, offset, musicType](auto response) {
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);
            if (result == nullptr) {
                return false;
            }
            for (auto &record : result->getResult()) {
                musicFilesViewCache.records.push_back(record);
            }
            musicFilesViewCache.recordsOffset = offset;
            musicFilesViewCache.recordsCount  = getRecordsCount();

            if (viewUpdateCallback) {
                viewUpdateCallback(musicFilesViewCache.records, musicFilesViewCache.recordsCount);
            }
            updateCountCallback(musicType, result->getCount());
            return true;
        };

        auto query = std::make_unique<db::multimedia_files::query::GetLimitedByPaths>(
            std::vector<std::string>{pathPrefixes[musicType]}, offset, limit);
        auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
        task->setCallback(taskCallback);
        task->execute(application, this);
    }

    std::uint32_t RelaxationSongsRepository::getRecordsCount()
    {
        return std::accumulate(filesPerType.begin(),
                               filesPerType.end(),
                               0,
                               [](const std::uint32_t sum, const auto &record) { return sum + record.second; });
    }

    void RelaxationSongsRepository::getMusicFiles(
        std::uint32_t offset,
        std::uint32_t limit,
        const RelaxationSongsRepository::OnGetMusicFilesListCallback &callback)
    {
        const auto skipViewUpdate = []([[maybe_unused]] const auto &sound, [[maybe_unused]] auto count) {
            return true;
        };
        musicFilesViewCache.records.clear();
        std::uint32_t totalFilesCount{0};
        const auto lastFileIndex = offset + limit;

        if (filesPerType.empty()) {
            getMusicFiles(MusicType::Relaxation, offset, limit, callback);
        }

        for (const auto &[type, filesCount] : filesPerType) {
            totalFilesCount += filesCount;
            if (const auto newOffset = calculateOffset(offset, filesCount); newOffset != offset) {
                offset = newOffset;
            }
            else if (lastFileIndex <= totalFilesCount) {
                getMusicFiles(type, offset, limit, callback);
                break;
            }
            else {
                const auto filesToLoad = filesCount - offset;
                getMusicFiles(type, offset, filesToLoad, skipViewUpdate);
                std::tie(offset, limit) = calculateNewOffsetAndLimit(offset, limit, filesToLoad);
                offset                  = calculateOffset(offset, filesCount);
            }
        }
    }

} // namespace app::relaxation
