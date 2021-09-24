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

namespace app::music_player
{
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

        virtual void getMusicFilesList(std::uint32_t offset,
                                       std::uint32_t limit,
                                       const OnGetMusicFilesListCallback &callback) = 0;
        virtual std::size_t getFileIndex(const std::string &filePath) const        = 0;
        virtual std::string getNextFilePath(const std::string &filePath) const     = 0;
        virtual std::string getPreviousFilePath(const std::string &filePath) const = 0;
    };

    class SongsRepository : public AbstractSongsRepository, public app::AsyncCallbackReceiver
    {
      public:
        explicit SongsRepository(ApplicationCommon *application, std::unique_ptr<AbstractTagsFetcher> tagsFetcher);

        void getMusicFilesList(std::uint32_t offset,
                               std::uint32_t limit,
                               const OnGetMusicFilesListCallback &callback) override;
        std::size_t getFileIndex(const std::string &filePath) const override;
        std::string getNextFilePath(const std::string &filePath) const override;
        std::string getPreviousFilePath(const std::string &filePath) const override;

      private:
        ApplicationCommon *application;

        std::unique_ptr<AbstractTagsFetcher> tagsFetcher;
        std::vector<db::multimedia_files::MultimediaFilesRecord> musicFiles;
    };
} // namespace app::music_player
