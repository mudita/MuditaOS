// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/models/SongsRepository.hpp>
#include <memory>
namespace app::relaxation
{
    enum class MusicType
    {
        Relaxation,
        User
    };

    class RelaxationSongsRepository : public app::AsyncCallbackReceiver
    {
      public:
        using OnGetMusicFilesListCallback =
            std::function<bool(const std::vector<db::multimedia_files::MultimediaFilesRecord> &, std::uint32_t)>;

        explicit RelaxationSongsRepository(ApplicationCommon *application,
                                           const std::map<MusicType, std::string> &pathPrefixes);
        void init();
        void getMusicFiles(std::uint32_t offset,
                           std::uint32_t limit,
                           const OnGetMusicFilesListCallback &viewUpdateCallback);

        std::uint32_t getRecordsCount();

      private:
        ApplicationCommon *application;
        std::map<MusicType, std::string> pathPrefixes;
        std::map<MusicType, std::uint32_t> filesPerType;
        music::FilesCache musicFilesViewCache;
        void updateFilesCount(const MusicType &type, const std::string &path);
        bool updateCountCallback(const MusicType &type, std::uint32_t count);
        void getMusicFiles(const MusicType &musicType,
                           std::uint32_t offset,
                           std::uint32_t limit,
                           const OnGetMusicFilesListCallback &viewUpdateCallback);
    };
} // namespace app::relaxation
