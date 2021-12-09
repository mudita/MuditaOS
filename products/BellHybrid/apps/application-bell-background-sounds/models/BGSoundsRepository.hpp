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

namespace app::bgSounds
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

    class BGSoundsTagsFetcher : public AbstractTagsFetcher
    {
      public:
        explicit BGSoundsTagsFetcher(ApplicationCommon *application);

        std::optional<tags::fetcher::Tags> getFileTags(const std::string &filePath) const final;

      private:
        ApplicationCommon *application = nullptr;
    };

    class AbstractSoundsRepository
    {
      public:
        virtual ~AbstractSoundsRepository() noexcept = default;

        virtual std::vector<tags::fetcher::Tags> getMusicFilesList() const = 0;
    };

    class BGSoundsRepository : public AbstractSoundsRepository, public app::AsyncCallbackReceiver
    {
        static constexpr auto bgsoundsSubfolderPath = "assets/audio/bell/bg_sounds";

      public:
        using OnGetMusicFilesListCallback =
            std::function<bool(const std::vector<db::multimedia_files::MultimediaFilesRecord> &, unsigned int)>;

        BGSoundsRepository(ApplicationCommon *application,
                           std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                           std::string musicFolderName = purefs::dir::getCurrentOSPath() / bgsoundsSubfolderPath);

        void getMusicFilesList(std::uint32_t offset, std::uint32_t limit, const OnGetMusicFilesListCallback &callback);

      private:
        ApplicationCommon *application = nullptr;
        std::unique_ptr<AbstractTagsFetcher> tagsFetcher;
        std::string musicFolderName;
        FilesCache musicFilesViewCache;
        std::vector<tags::fetcher::Tags> musicFiles;
    };
} // namespace app::bgSounds
