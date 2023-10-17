// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultimediaFilesRecord.hpp"
#include "BaseInterface.hpp"

#include <databases/MultimediaFilesDB.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesAdd.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesEdit.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesGet.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesRemove.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesCount.hpp>

namespace db::multimedia_files
{

    MultimediaFilesRecordInterface::MultimediaFilesRecordInterface(MultimediaFilesDB *database) : database(database)
    {}

    std::unique_ptr<db::QueryResult> MultimediaFilesRecordInterface::runQuery(std::shared_ptr<db::Query> query)
    {
        if (query == nullptr) {
            return nullptr;
        }

        if (typeid(*query) == typeid(query::Add)) {
            return runQueryImplAdd(std::static_pointer_cast<query::Add>(query));
        }
        if (typeid(*query) == typeid(query::Edit)) {
            return runQueryImplEdit(std::static_pointer_cast<query::Edit>(query));
        }
        if (typeid(*query) == typeid(query::Get)) {
            return runQueryImplGet(std::static_pointer_cast<query::Get>(query));
        }
        if (typeid(*query) == typeid(query::GetLimited)) {
            return runQueryImplGetLimited(std::static_pointer_cast<query::GetLimited>(query));
        }
        if (typeid(*query) == typeid(query::GetLimitedByPaths)) {
            return runQueryImplGetLimited(std::static_pointer_cast<query::GetLimitedByPaths>(query));
        }
        if (typeid(*query) == typeid(query::Remove)) {
            return runQueryImplRemove(std::static_pointer_cast<query::Remove>(query));
        }
        if (typeid(*query) == typeid(query::RemoveByPath)) {
            return runQueryImplRemoveByPath(std::static_pointer_cast<query::RemoveByPath>(query));
        }
        if (typeid(*query) == typeid(query::RemoveAll)) {
            return runQueryImplRemoveAll(std::static_pointer_cast<query::RemoveAll>(query));
        }
        if (typeid(*query) == typeid(query::GetCount)) {
            return runQueryImplGetCount(std::static_pointer_cast<query::GetCount>(query));
        }
        if (typeid(*query) == typeid(query::GetCountArtists)) {
            return runQueryImplGetCountArtists(std::static_pointer_cast<query::GetCountArtists>(query));
        }
        if (typeid(*query) == typeid(query::GetArtistsLimited)) {
            return runQueryImplGetArtistsLimited(std::static_pointer_cast<query::GetArtistsLimited>(query));
        }
        if (typeid(*query) == typeid(query::GetCountAlbums)) {
            return runQueryImplGetCountAlbums(std::static_pointer_cast<query::GetCountAlbums>(query));
        }
        if (typeid(*query) == typeid(query::GetAlbumsLimited)) {
            return runQueryImplGetAlbumsLimited(std::static_pointer_cast<query::GetAlbumsLimited>(query));
        }
        if (typeid(*query) == typeid(query::GetLimitedForArtist)) {
            return runQueryImplGetLimited(std::static_pointer_cast<query::GetLimitedForArtist>(query));
        }
        if (typeid(*query) == typeid(query::GetCountForArtist)) {
            return runQueryImplGetCount(std::static_pointer_cast<query::GetCountForArtist>(query));
        }
        if (typeid(*query) == typeid(query::GetLimitedForAlbum)) {
            return runQueryImplGetLimited(std::static_pointer_cast<query::GetLimitedForAlbum>(query));
        }
        if (typeid(*query) == typeid(query::GetCountForAlbum)) {
            return runQueryImplGetCount(std::static_pointer_cast<query::GetCountForAlbum>(query));
        }
        if (typeid(*query) == typeid(query::AddOrEdit)) {
            return runQueryImplAddOrEdit(std::static_pointer_cast<query::AddOrEdit>(query));
        }
        if (typeid(*query) == typeid(query::GetByPath)) {
            return runQueryImplGetByPath(std::static_pointer_cast<query::GetByPath>(query));
        }
        return nullptr;
    }

    std::unique_ptr<query::AddResult> MultimediaFilesRecordInterface::runQueryImplAdd(
        const std::shared_ptr<query::Add> &query)
    {
        const auto result = database->files.add(query->getRecord());

        auto response = std::make_unique<query::AddResult>(result);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::AddOrEditResult> MultimediaFilesRecordInterface::runQueryImplAddOrEdit(
        const std::shared_ptr<query::AddOrEdit> &query)
    {
        const auto result = database->files.addOrUpdate(query->getRecord(), query->getOldPath());

        auto response = std::make_unique<query::AddOrEditResult>(result);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::EditResult> MultimediaFilesRecordInterface::runQueryImplEdit(
        const std::shared_ptr<query::Edit> &query)
    {
        const auto result = database->files.update(query->getRecord());

        auto response = std::make_unique<query::EditResult>(result);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::GetResult> MultimediaFilesRecordInterface::runQueryImplGet(
        const std::shared_ptr<query::Get> &query)
    {
        const auto record = database->files.getById(query->id);

        auto response = std::make_unique<query::GetResult>(record);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::GetResult> MultimediaFilesRecordInterface::runQueryImplGetByPath(
        const std::shared_ptr<query::GetByPath> &query)
    {
        const auto record = database->files.getByPath(query->path);

        auto response = std::make_unique<query::GetResult>(record);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::GetLimitedResult> MultimediaFilesRecordInterface::runQueryImplGetLimited(
        const std::shared_ptr<query::GetLimited> &query)
    {
        const auto records = database->files.getLimitOffset(query->offset, query->limit);

        auto response = std::make_unique<query::GetLimitedResult>(records, database->files.count());
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<query::RemoveResult> MultimediaFilesRecordInterface::runQueryImplRemove(
        const std::shared_ptr<query::Remove> &query)
    {
        const bool ret = database->files.removeById(query->id);
        auto response  = std::make_unique<query::RemoveResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::RemoveResult> MultimediaFilesRecordInterface::runQueryImplRemoveByPath(
        const std::shared_ptr<query::RemoveByPath> &query)
    {
        const bool ret = database->files.removeByField(TableFields::path, query->path.c_str());
        auto response  = std::make_unique<query::RemoveResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::RemoveResult> MultimediaFilesRecordInterface::runQueryImplRemoveAll(
        const std::shared_ptr<query::RemoveAll> &query)
    {
        const bool ret = database->files.removeAll();
        auto response  = std::make_unique<query::RemoveResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<query::GetCountResult> MultimediaFilesRecordInterface::runQueryImplGetCount(
        const std::shared_ptr<query::GetCount> &query)
    {
        const auto ret = database->files.count();
        auto response  = std::make_unique<query::GetCountResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetCountResult> MultimediaFilesRecordInterface::
        runQueryImplGetCountArtists(const std::shared_ptr<db::multimedia_files::query::GetCountArtists> &query)
    {
        const auto ret = database->files.countArtists();
        auto response  = std::make_unique<query::GetCountResult>(ret);
        response->setRequestQuery(query);
        return response;
    }
    std::unique_ptr<db::multimedia_files::query::GetArtistsLimitedResult> MultimediaFilesRecordInterface::
        runQueryImplGetArtistsLimited(const std::shared_ptr<db::multimedia_files::query::GetArtistsLimited> &query)
    {
        const auto records = database->files.getArtistsLimitOffset(query->offset, query->limit);

        auto response = std::make_unique<query::GetArtistsLimitedResult>(records, database->files.countArtists());
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetCountResult> MultimediaFilesRecordInterface::
        runQueryImplGetCountAlbums(const std::shared_ptr<db::multimedia_files::query::GetCountAlbums> &query)
    {
        const auto ret = database->files.countAlbums();
        auto response  = std::make_unique<query::GetCountResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetAlbumsLimitedResult> MultimediaFilesRecordInterface::
        runQueryImplGetAlbumsLimited(const std::shared_ptr<db::multimedia_files::query::GetAlbumsLimited> &query)
    {
        const auto records = database->files.getAlbumsLimitOffset(query->offset, query->limit);

        auto response = std::make_unique<query::GetAlbumsLimitedResult>(records, database->files.countAlbums());
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetLimitedResult> MultimediaFilesRecordInterface::
        runQueryImplGetLimited(const std::shared_ptr<db::multimedia_files::query::GetLimitedForArtist> &query)
    {
        const auto records = database->files.getLimitOffset(query->artist, query->offset, query->limit);

        auto response = std::make_unique<query::GetLimitedResult>(records, database->files.count());
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetCountResult> MultimediaFilesRecordInterface::runQueryImplGetCount(
        const std::shared_ptr<db::multimedia_files::query::GetCountForArtist> &query)
    {
        const auto ret = database->files.count(query->artist);
        auto response  = std::make_unique<query::GetCountResult>(ret);
        response->setRequestQuery(query);
        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetLimitedResult> MultimediaFilesRecordInterface::
        runQueryImplGetLimited(const std::shared_ptr<db::multimedia_files::query::GetLimitedForAlbum> &query)
    {
        const auto records = database->files.getLimitOffset(query->album, query->offset, query->limit);

        auto response = std::make_unique<query::GetLimitedResult>(records, database->files.count());
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetLimitedResult> MultimediaFilesRecordInterface::
        runQueryImplGetLimited(const std::shared_ptr<db::multimedia_files::query::GetLimitedByPaths> &query)
    {
        const auto records = database->files.getLimitOffsetByPaths(query->paths, query->offset, query->limit);
        auto response      = std::make_unique<query::GetLimitedResult>(records, database->files.count(query->paths));
        response->setRequestQuery(query);

        return response;
    }

    std::unique_ptr<db::multimedia_files::query::GetCountResult> MultimediaFilesRecordInterface::runQueryImplGetCount(
        const std::shared_ptr<db::multimedia_files::query::GetCountForAlbum> &query)
    {
        const auto ret = database->files.count(query->album);
        auto response  = std::make_unique<query::GetCountResult>(ret);
        response->setRequestQuery(query);
        return response;
    }
} // namespace db::multimedia_files
