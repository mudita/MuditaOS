// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <Audio/decoder/Decoder.hpp>
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

        virtual std::optional<audio::Tags> getFileTags(const std::string &filePath) const = 0;
    };

    class ServiceAudioTagsFetcher : public AbstractTagsFetcher
    {
      public:
        explicit ServiceAudioTagsFetcher(Application *application);

        std::optional<audio::Tags> getFileTags(const std::string &filePath) const final;

      private:
        Application *application = nullptr;
    };

    class AbstractSongsRepository
    {
      public:
        virtual ~AbstractSongsRepository() noexcept = default;

        virtual void scanMusicFilesList()                                   = 0;
        virtual std::vector<audio::Tags> getMusicFilesList() const          = 0;
        virtual std::size_t getFileIndex(const std::string &filePath) const = 0;
    };

    class SongsRepository : public AbstractSongsRepository
    {
        static constexpr auto musicSubfolderName = "music";

      public:
        explicit SongsRepository(std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                 std::string musicFolderName = purefs::dir::getUserDiskPath() / musicSubfolderName);

        void scanMusicFilesList() override;
        std::vector<audio::Tags> getMusicFilesList() const override;
        std::size_t getFileIndex(const std::string &filePath) const override;

      private:
        std::unique_ptr<AbstractTagsFetcher> tagsFetcher;
        std::string musicFolderName;
        std::vector<audio::Tags> musicFiles;
    };
} // namespace app::music_player
