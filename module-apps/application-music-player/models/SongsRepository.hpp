// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <purefs/filesystem_paths.hpp>

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
        virtual ~AbstractSongsRepository() noexcept = default;

        virtual void scanMusicFilesList()                                          = 0;
        virtual std::vector<tags::fetcher::Tags> getMusicFilesList() const         = 0;
        virtual std::size_t getFileIndex(const std::string &filePath) const        = 0;
        virtual std::string getNextFilePath(const std::string &filePath) const     = 0;
        virtual std::string getPreviousFilePath(const std::string &filePath) const = 0;
    };

    class SongsRepository : public AbstractSongsRepository
    {
        static constexpr auto musicSubfolderName = "music";

      public:
        explicit SongsRepository(std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                 std::string musicFolderName = purefs::dir::getUserDiskPath() / musicSubfolderName);

        void scanMusicFilesList() override;
        std::vector<tags::fetcher::Tags> getMusicFilesList() const override;
        std::size_t getFileIndex(const std::string &filePath) const override;
        std::string getNextFilePath(const std::string &filePath) const override;
        std::string getPreviousFilePath(const std::string &filePath) const override;

      private:
        std::unique_ptr<AbstractTagsFetcher> tagsFetcher;
        std::string musicFolderName;
        std::vector<tags::fetcher::Tags> musicFiles;
    };
} // namespace app::music_player
