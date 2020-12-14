// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace FileIndexer::Statements
{

    constexpr auto getFilesCount = R"sql(
                        SELECT COUNT(FT.file_id) AS FILE_PATH_EXISTS FROM  file_tab AS FT;
                        )sql";

    constexpr auto getMetadataCount = R"sql(
                        SELECT COUNT(MT.file_id) AS FILE_PATH_EXISTS FROM  metadata_tab AS MT;
                        )sql";

    constexpr auto checkFileExists = R"sql(
                        SELECT COUNT(size) AS FILE_PATH_EXISTS FROM  file_tab AS FT
                        WHERE FT.path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getFileIdByPath = R"sql(
                        SELECT FT.file_id FROM file_tab AS FT
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
                        WHERE FT.directory = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto insertFileInfo = R"sql(
                        INSERT OR REPLACE INTO file_tab (path, size, mime_type, mtime, directory, file_type) VALUES
                        ('%q', '%lu', '%lu' , '%lu', '%q', '%lu') ;
                        )sql";

    constexpr auto updateFileInfo = R"sql(
                        UPDATE file_tab SET path= '%q',
                        size= '%lu',
                        mime_type= '%lu',
                        mtime= '%lu',
                        directory ='%q',
                        file_type= '%lu'
                        WHERE file_id = '%lu' ;
                        )sql";

    constexpr auto deleteFileById = R"sql(
                        DELETE FROM file_tab AS FT
                        WHERE FT.file_id = '%lu';
                        DELETE FROM metadata_tab AS MT
                        WHERE MT.file_id = '%lu';
                        )sql";

    constexpr auto deleteAllFileInfoInDir = R"sql(
                        DELETE FROM file_tab AS FT
                        WHERE FT.directory = '%q';
                        )sql";

    constexpr auto getFilesIdByDir = R"sql(
                        SELECT file_id FROM  file_tab AS FT
                        WHERE FT.directory = '%q'
                        COLLATE NOCASE;
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

    constexpr auto updatePropertyValue = R"sql(
                        UPDATE metadata_tab SET value = '%q'
                        WHERE file_id= '%lu' AND property = '%q' ;
                        )sql";

    constexpr auto setNotification = R"sql(
                        INSERT OR REPLACE INTO notifications_tab (path, service) VALUES
                        ( '%q' , '%q' ) ;
                        )sql";

    constexpr auto updateNotification = R"sql(
                        UPDATE notifications_tab SET path = '%q', service = '%q'
                        WHERE id = '%lu';
                        )sql";

    constexpr auto clearNotificationdRow = R"sql(
                        DELETE FROM notifications_tab
                        WHERE path = '%q' AND service = '%q';
                        )sql";

    constexpr auto getAllNotifications = R"sql(
                        SELECT id, path, service
                        FROM notifications_tab  AS NT;
                        )sql";

    constexpr auto getNotification = R"sql(
                        SELECT id, path, service
                        FROM notifications_tab  AS NT
                        WHERE path= '%q' AND service = '%q';
                        )sql";

} // namespace FileIndexer::Statements
