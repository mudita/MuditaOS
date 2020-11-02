// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace FileIndexer::Statements
{
    constexpr auto checkFileExists = R"sql(
                        SELECT COUNT(size) AS FILE_PATH_EXISTS FROM  file_tab AS FT
                        WHERE FT.path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFileIdByPath = R"sql(
                        SELECT FT.file_id file_tab AS FT
                        WHERE FT.path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFilePathById = R"sql(
                        SELECT FT.path  FROM  file_tab AS FT
                        WHERE FT.file_id = '%lu'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFileInfoByPath = R"sql(
                        SELECT file_id, path, size, mime_type, mtime, directory ,file_type  FROM  file_tab AS FT
                        WHERE FT.path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFileInfoById = R"sql(
                        SELECT file_id, path, size, mime_type, mtime, directory, file_type   FROM  file_tab AS FT
                        WHERE FT.file_id = '%lu'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFilesByDir = R"sql(
                        SELECT file_id, path, size, mime_type, mtime, directory, file_type   FROM  file_tab AS FT
                        WHERE FT.directory = '%lu'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto insertFileInfo = R"sql(
                        INSERT OR REPLACE INTO file_tab (path, size, file_type, mtime, directory, file_type) VALUES
                        ( '%q', '%lu', '%lu' , '%lu', '%q', '%lu') ;
                        )sql";

    constexpr auto getPropertyValue = R"sql(
                         SELECT MT.file_id, MT.property, MT.value
                         FROM metadata_tab AS MT, file_tab AS FT
                         WHERE MT.property = '%q' AND FT.path = '%q' AND  FT.file_id = MT.file_id
                         COLLATE NOCASE;
                         )sql";

    constexpr auto getAllProperties = R"sql(
                         SELECT MT.file_id, MT.property, MT.value
                         FROM metadata_tab AS MT, file_tab AS FT
                         WHERE FT.path = '%q' AND  FT.file_id = MT.file_id
                         COLLATE NOCASE;
                         )sql";

    constexpr auto insertPropertyValue = R"sql(
                        INSERT OR REPLACE INTO metadata_tab (file_id, property, value) VALUES
                        ( '%lu', '%q', '%q' ) ;
                        )sql";

} // namespace FileIndexer::Statements
