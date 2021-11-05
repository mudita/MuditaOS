// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <cstddef>

namespace app::music
{
    struct FilesCache
    {
        std::vector<db::multimedia_files::MultimediaFilesRecord> records{};
        std::uint32_t recordsOffset{0};
        std::uint32_t recordsCount{0};
    };

    class AbstractTagsFetcher
    {
      public:
        virtual ~AbstractTagsFetcher() noexcept = default;

        virtual std::optional<tags::fetcher::Tags> getFileTags(const std::string &filePath) const = 0;
    };

    class ServiceAudioTagsFetcher : public AbstractTagsFetcher
    {
      public:
        explicit ServiceAudioTagsFetcher(ApplicationCommon *application);

        std::optional<tags::fetcher::Tags> getFileTags(const std::string &filePath) const final;

      private:
        ApplicationCommon *application = nullptr;
    };

    class AbstractSongsRepository
    {
      public:
        using OnGetMusicFilesListCallback =
            std::function<bool(const std::vector<db::multimedia_files::MultimediaFilesRecord> &, unsigned int)>;

        virtual ~AbstractSongsRepository() noexcept = default;

        virtual void initCache()                                                    = 0;
        virtual void getMusicFilesList(std::uint32_t offset,
                                       std::uint32_t limit,
                                       const OnGetMusicFilesListCallback &callback) = 0;
        virtual std::string getNextFilePath(const std::string &filePath) const      = 0;
        virtual std::string getPreviousFilePath(const std::string &filePath) const  = 0;
        virtual std::optional<db::multimedia_files::MultimediaFilesRecord> getRecord(
            const std::string &filePath) const                     = 0;
        virtual void updateRepository(const std::string &filePath) = 0;
    };

    class SongsRepository : public AbstractSongsRepository, public app::AsyncCallbackReceiver
    {
      public:
        explicit SongsRepository(ApplicationCommon *application,
                                 std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                 std::string pathPrefix);

        void initCache();
        void getMusicFilesList(std::uint32_t offset, std::uint32_t limit, const OnGetMusicFilesListCallback &callback);
        std::string getNextFilePath(const std::string &filePath) const override;
        std::string getPreviousFilePath(const std::string &filePath) const override;
        std::optional<db::multimedia_files::MultimediaFilesRecord> getRecord(
            const std::string &filePath) const override;

        /// If the currently playing track is not included in the musicFilesCache then we request new data of the size
        /// of the cache otherwise if the index number of the currently playing track exceeds the upper or lower
        /// cacheThreshold, then we request the next records. See doc/SongsRepository.md for more information.
        void updateRepository(const std::string &filePath) override;

      private:
        using OnUpdateMusicFilesListCallback = std::function<bool(
            const std::vector<db::multimedia_files::MultimediaFilesRecord> &, unsigned int, std::uint32_t)>;

        ApplicationCommon *application;

        std::unique_ptr<AbstractTagsFetcher> tagsFetcher;

        std::string pathPrefix;

        /// collection of music files displayed in the list view
        FilesCache musicFilesViewCache;
        /// collection of music files currently playing
        FilesCache musicFilesModelCache;

        std::size_t getCachedFileIndex(const std::string &filePath) const;
        std::uint32_t calculateOffset();

        /// overwrite the musicFiles view cache with new data from the given index
        void updateMusicFilesList(std::uint32_t offset,
                                  std::uint32_t limit,
                                  const OnUpdateMusicFilesListCallback &callback);

        /// overwrite musicFiles model cache with new data from zero index
        bool initCacheCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                               unsigned int repoRecordsCount);
        /// overwrite the musicFiles model cache with new data from the given index
        bool newDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                             unsigned int repoRecordsCount,
                             std::uint32_t offset);
        /// adding new data to the musicFiles model cache at the end of the list, with the first records being deleted
        /// so that the cache size is not changed
        bool newBackDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                 unsigned int repoRecordsCount,
                                 std::uint32_t offset);
        /// adding new data to the musicFiles model cache to the top of the list, but the last records are removed so
        /// that the cache size is not changed
        bool newFrontDataCallback(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned int repoRecordsCount,
                                  std::uint32_t offset);
    };
} // namespace app::music
