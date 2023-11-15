﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
    class AddOrEdit;
    class AddOrEditResult;
    class AddResult;
    class Edit;
    class EditResult;
    class Get;
    class GetAlbumsLimited;
    class GetAlbumsLimitedResult;
    class GetArtistsLimited;
    class GetArtistsLimitedResult;
    class GetByPath;
    class GetCount;
    class GetCountAlbums;
    class GetCountArtists;
    class GetCountForAlbum;
    class GetCountForArtist;
    class GetCountResult;
    class GetLimited;
    class GetLimitedByPaths;
    class GetLimitedForAlbum;
    class GetLimitedForArtist;
    class GetLimitedResult;
    class GetResult;
    class Remove;
    class RemoveAll;
    class RemoveAll;
    class RemoveByPath;
    class RemoveResult;
    class GetCountForPath;
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
        std::unique_ptr<db::multimedia_files::query::AddOrEditResult> runQueryImplAddOrEdit(
            const std::shared_ptr<db::multimedia_files::query::AddOrEdit> &query);
        std::unique_ptr<db::multimedia_files::query::GetResult> runQueryImplGet(
            const std::shared_ptr<db::multimedia_files::query::Get> &query);
        std::unique_ptr<db::multimedia_files::query::GetLimitedResult> runQueryImplGetLimited(
            const std::shared_ptr<db::multimedia_files::query::GetLimited> &query);
        std::unique_ptr<db::multimedia_files::query::GetLimitedResult> runQueryImplGetLimited(
            const std::shared_ptr<db::multimedia_files::query::GetLimitedByPaths> &query);
        std::unique_ptr<db::multimedia_files::query::RemoveResult> runQueryImplRemove(
            const std::shared_ptr<db::multimedia_files::query::Remove> &query);
        std::unique_ptr<db::multimedia_files::query::RemoveResult> runQueryImplRemoveAll(
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
        std::unique_ptr<db::multimedia_files::query::GetResult> runQueryImplGetByPath(
            const std::shared_ptr<db::multimedia_files::query::GetByPath> &query);
        std::unique_ptr<db::multimedia_files::query::RemoveResult> runQueryImplRemoveByPath(
            const std::shared_ptr<db::multimedia_files::query::RemoveByPath> &query);
        std::unique_ptr<db::multimedia_files::query::GetCountResult> runQueryImplGetCountForPath(
            const std::shared_ptr<db::multimedia_files::query::GetCountForPath> &query);

        MultimediaFilesDB *database = nullptr;
    };
} // namespace db::multimedia_files
