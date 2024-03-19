// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsRepository.hpp"

#include <module-db/queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesCount.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesGetOffset.hpp>
#include <AsyncTask.hpp>
#include <algorithm>

namespace fs = std::filesystem;

namespace
{
    constexpr auto allowedExtensions = {".wav", ".mp3", ".flac"};
    constexpr auto defaultOffset{0};

    constexpr std::uint32_t calculateOffset(std::uint32_t offset, std::uint32_t filesCount)
    {
        if (offset >= filesCount) {
            return offset - filesCount;
        }
        return offset;
    }

    constexpr std::pair<std::uint32_t, std::uint32_t> calculateNewOffsetAndLimit(std::uint32_t offset,
                                                                                 std::uint32_t limit,
                                                                                 std::uint32_t loadedFiles)
    {
        const auto newLimit  = limit - loadedFiles;
        const auto newOffset = offset + loadedFiles;
        return {newOffset, newLimit};
    }

    constexpr db::multimedia_files::SortingBy transformSorting(SoundsRepository::SortingBy sorting)
    {
        switch (sorting) {
        case SoundsRepository::SortingBy::TrackIdAscending:
            return db::multimedia_files::SortingBy::TrackIdAscending;
        case SoundsRepository::SortingBy::TitleAscending:
        default:
            return db::multimedia_files::SortingBy::TitleAscending;
        }
    }
} // namespace

SimpleSoundsRepository::SimpleSoundsRepository(std::filesystem::path dirToScan)
{
    for (const auto &entry : std::filesystem::directory_iterator(dirToScan)) {
        processEntry(entry);
    }

    /// Sort entries by track ID
    std::sort(samples.begin(), samples.end(), [](const auto &a, const auto &b) { return a.track < b.track; });
}

std::optional<std::filesystem::path> SimpleSoundsRepository::titleToPath(const UTF8 &title) const
{
    const auto res =
        std::find_if(samples.begin(), samples.end(), [title](const auto &e) { return e.title == title.c_str(); });
    if (res != samples.end()) {
        return std::filesystem::path{res->filePath};
    }
    return {};
}

std::optional<UTF8> SimpleSoundsRepository::pathToTitle(std::filesystem::path path) const
{
    const auto res =
        std::find_if(samples.begin(), samples.end(), [&path](const auto &e) { return e.filePath == path; });
    if (res != samples.end()) {
        return res->title;
    }
    return {};
}

std::vector<UTF8> SimpleSoundsRepository::getSongTitles()
{
    std::vector<UTF8> ret;
    std::transform(samples.begin(), samples.end(), std::back_inserter(ret), [](const auto &e) { return e.title; });
    return ret;
}

void SimpleSoundsRepository::processEntry(const std::filesystem::recursive_directory_iterator::value_type &entry)
{
    if (fs::is_regular_file(entry)) {
        for (const auto &ext : allowedExtensions) {
            if (fs::path(entry).extension() == ext) {
                samples.emplace_back(tags::fetcher::fetchTags(fs::absolute(entry).string()));
            }
        }
    }
}

SoundsRepository::SoundsRepository(app::ApplicationCommon *application, const PathSorting &path)
    : app::AsyncCallbackReceiver{application}, application{application}
{
    paths.push_back({0, 0, path.sorting, path.prefix});
}

SoundsRepository::SoundsRepository(app::ApplicationCommon *application, const std::vector<PathSorting> &pathsVector)
    : app::AsyncCallbackReceiver{application}, application{application}
{
    std::uint32_t id = 0;
    for (const auto &[path, sorting] : pathsVector) {
        paths.push_back({id, 0, sorting, path});
        id++;
    }
}

void SoundsRepository::init(const std::string &savedPath, OnOffsetUpdateCallback offsetUpdateCallback)
{
    updateFilesCount();
    updateOffsetFromSavedPath(savedPath, std::move(offsetUpdateCallback));
}

void SoundsRepository::getMusicFiles(std::uint32_t offset,
                                     std::uint32_t limit,
                                     const OnGetMusicFilesListCallback &viewUpdateCallback)
{
    const auto skipViewUpdate = []([[maybe_unused]] const auto &sound, [[maybe_unused]] auto count) { return true; };
    musicFilesViewCache.records.clear();
    std::uint32_t totalFilesCount{0};
    const auto lastFileIndex = offset + limit;
    if (getFilesCount() == 0 && !paths.empty()) {
        getMusicFiles(paths[0].prefix, paths[0].sorting, offset, limit, viewUpdateCallback);
    }

    for (const auto &[id, filesCount, sorting, path] : paths) {
        if (filesCount == 0) {
            continue;
        }
        totalFilesCount += filesCount;
        if (const auto newOffset = calculateOffset(offset, filesCount); newOffset != offset) {
            offset = newOffset;
        }
        else if (lastFileIndex <= totalFilesCount) {
            getMusicFiles(path, sorting, offset, limit, viewUpdateCallback);
            break;
        }
        else {
            const auto filesToLoad = filesCount - offset;
            getMusicFiles(path, sorting, offset, filesToLoad, skipViewUpdate);
            std::tie(offset, limit) = calculateNewOffsetAndLimit(offset, limit, filesToLoad);
            offset                  = calculateOffset(offset, filesCount);
        }
    }
}

void SoundsRepository::getMusicFiles(const std::string &path,
                                     const SortingBy sorting,
                                     const std::uint32_t offset,
                                     const std::uint32_t limit,
                                     const OnGetMusicFilesListCallback &viewUpdateCallback)
{
    auto taskCallback = [this, viewUpdateCallback, offset](auto response) {
        auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(response);
        if (result == nullptr) {
            return false;
        }
        for (auto &record : result->getResult()) {
            musicFilesViewCache.records.push_back(record);
        }
        musicFilesViewCache.recordsOffset = offset;
        musicFilesViewCache.recordsCount  = getFilesCount();

        if (viewUpdateCallback) {
            viewUpdateCallback(musicFilesViewCache.records, musicFilesViewCache.recordsCount);
        }
        return true;
    };

    const auto sortBy = transformSorting(sorting);
    auto query        = std::make_unique<db::multimedia_files::query::GetLimitedByPaths>(
        std::vector<std::string>{path}, offset, limit, sortBy);
    auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
    task->setCallback(taskCallback);
    task->execute(application, this);
}

std::uint32_t SoundsRepository::getFilesCount()
{
    return std::accumulate(
        paths.begin(), paths.end(), 0, [](const std::uint32_t sum, const auto &record) { return sum + record.count; });
}

void SoundsRepository::updateFilesCount()
{
    for (const auto &path : paths) {
        updateFilesCount(path.id, path.prefix);
    }
}

void SoundsRepository::updateFilesCount(const std::uint32_t id, const std::string &path)
{
    auto query = std::make_unique<db::multimedia_files::query::GetCountForPath>(path);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
    task->setCallback([this, id](auto response) {
        auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(response);
        if (result == nullptr) {
            return false;
        }
        return updateCountCallback(id, result->getCount());
    });
    task->execute(application, this);
}

bool SoundsRepository::updateCountCallback(const std::uint32_t id, const std::uint32_t count)
{
    const auto it = std::find_if(paths.begin(), paths.end(), [id](auto path) { return path.id == id; });
    if (it != paths.end()) {
        (*it).count = count;
        return true;
    }
    return false;
}

void SoundsRepository::updateOffsetFromSavedPath(const std::string &savedPath,
                                                 OnOffsetUpdateCallback offsetUpdateCallback)
{
    if (offsetUpdateCallback == nullptr) {
        return;
    }
    const auto path = getPathDetails(savedPath);
    if (!path.has_value()) {
        offsetUpdateCallback(defaultOffset);
        return;
    }

    auto query = std::make_unique<db::multimedia_files::query::GetOffsetByPath>(
        path->prefix, savedPath, transformSorting(path->sorting));
    auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::MultimediaFiles);
    task->setCallback([this, offsetUpdateCallback, savedPath](auto response) {
        auto result = dynamic_cast<db::multimedia_files::query::GetOffsetResult *>(response);
        if (result == nullptr || result->getResult().path != savedPath) {
            offsetUpdateCallback(defaultOffset);
            return false;
        }
        const auto calculatedOffset = calculateOffsetFromDB(result->getResult().offset, savedPath);
        offsetUpdateCallback(calculatedOffset.has_value() ? calculatedOffset.value() : defaultOffset);
        return true;
    });
    task->execute(application, this);
}

std::optional<SoundsRepository::PathDetails> SoundsRepository::getPathDetails(const std::string &songPath)
{
    for (const auto &path : paths) {
        if (songPath.find(path.prefix) != std::string::npos) {
            return path;
        }
    }
    return std::nullopt;
}

std::optional<std::uint32_t> SoundsRepository::calculateOffsetFromDB(std::uint32_t offset, const std::string &songPath)
{
    // the offset returned by the database is incremented from 1,
    // while the offset used in listView is from 0
    if (offset > 0) {
        offset--;
    }
    for (const auto &path : paths) {
        if (songPath.find(path.prefix) != std::string::npos) {
            return offset;
        }
        offset += path.count;
    }
    return std::nullopt;
}
