// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Record.hpp"

#include <Tables/MultimediaFilesTable.hpp>

#include <string>

namespace db::multimedia_files
{
    class MultimediaFilesDB;
} // namespace db::multimedia_files

namespace db::multimedia_files::query
{
    class Add;
    class AddResult;
    class Edit;
    class EditResult;
    class Get;
    class GetAlbumsLimited;
    class GetAlbumsLimitedResult;
    class GetArtistsLimited;
    class GetArtistsLimitedResult;
    class GetCount;
    class GetCountAlbums;
    class GetCountArtists;
    class GetCountForAlbum;
    class GetCountForArtist;
    class GetCountResult;
    class GetLimited;
    class GetLimitedForAlbum;
    class GetLimitedForArtist;
    class GetLimitedResult;
    class GetResult;
    class Remove;
    class RemoveAll;
    class RemoveAllResult;
    class RemoveResult;
} // namespace db::multimedia_files::query

namespace db::multimedia_files
{

    using MultimediaFilesRecordField = TableFields;

    using MultimediaFilesRecord = TableRow;

    class MultimediaFilesRecordInterface : public RecordInterface<MultimediaFilesRecord, MultimediaFilesRecordField>
    {
      public:
        explicit MultimediaFilesRecordInterface(MultimediaFilesDB *db);

        std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

      private:
        std::unique_ptr<db::multimedia_files::query::AddResult> runQueryImplAdd(
            const std::shared_ptr<db::multimedia_files::query::Add> &query);
        std::unique_ptr<db::multimedia_files::query::EditResult> runQueryImplEdit(
            const std::shared_ptr<db::multimedia_files::query::Edit> &query);
        std::unique_ptr<db::multimedia_files::query::GetResult> runQueryImplGet(
            const std::shared_ptr<db::multimedia_files::query::Get> &query);
        std::unique_ptr<db::multimedia_files::query::GetLimitedResult> runQueryImplGetLimited(
            const std::shared_ptr<db::multimedia_files::query::GetLimited> &query);
        std::unique_ptr<db::multimedia_files::query::RemoveResult> runQueryImplRemove(
            const std::shared_ptr<db::multimedia_files::query::Remove> &query);
        std::unique_ptr<db::multimedia_files::query::RemoveAllResult> runQueryImplRemoveAll(
            const std::shared_ptr<db::multimedia_files::query::RemoveAll> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCount(
            const std::shared_ptr<db::multimedia_files::query::GetCount> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCountArtists(
            const std::shared_ptr<db::multimedia_files::query::GetCountArtists> &query);
        std::unique_ptr<db::multimedia_files::query::GetArtistsLimitedResult> runQueryImplGetArtistsLimited(
            const std::shared_ptr<db::multimedia_files::query::GetArtistsLimited> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCountAlbums(
            const std::shared_ptr<db::multimedia_files::query::GetCountAlbums> &query);
        std::unique_ptr<db::multimedia_files::query::GetAlbumsLimitedResult> runQueryImplGetAlbumsLimited(
            const std::shared_ptr<db::multimedia_files::query::GetAlbumsLimited> &query);
        std::unique_ptr<db::multimedia_files::query::GetLimitedResult> runQueryImplGetLimited(
            const std::shared_ptr<db::multimedia_files::query::GetLimitedForArtist> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCount(
            const std::shared_ptr<db::multimedia_files::query::GetCountForArtist> &query);
        std::unique_ptr<db::multimedia_files::query::GetLimitedResult> runQueryImplGetLimited(
            const std::shared_ptr<db::multimedia_files::query::GetLimitedForAlbum> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCount(
            const std::shared_ptr<db::multimedia_files::query::GetCountForAlbum> &query);

      private:
        MultimediaFilesDB *database = nullptr;
    };
} // namespace db::multimedia_files
