// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <algorithm>
#include <log/log.hpp>
#include <time/ScopedTime.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <apps-common/models/SongsRepository.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>

#include <filesystem>

namespace app::bgSounds
{
    BGSoundsTagsFetcher::BGSoundsTagsFetcher(ApplicationCommon *application) : application(application)
    {}

    std::optional<tags::fetcher::Tags> BGSoundsTagsFetcher::getFileTags(const std::string &filePath) const
    {
        return tags::fetcher::fetchTags(filePath);
    }

    BGSoundsRepository::BGSoundsRepository(ApplicationCommon *application,
                                           std::unique_ptr<AbstractTagsFetcher> tagsFetcher,
                                           std::string musicFolderName)
        : app::AsyncCallbackReceiver{application}, application{application}, tagsFetcher(std::move(tagsFetcher)),
          musicFolderName(std::move(musicFolderName))
    {}

    void BGSoundsRepository::getMusicFilesList(std::uint32_t offset,
                                               std::uint32_t limit,
                                               const OnGetMusicFilesListCallback &callback)
    {
        auto query = std::make_unique<db::multimedia_files::query::GetLimited>(offset, limit);
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
} // namespace app::bgSounds
