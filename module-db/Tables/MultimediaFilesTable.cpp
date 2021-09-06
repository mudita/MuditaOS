// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultimediaFilesTable.hpp"

#include <Database/QueryResult.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

namespace db::multimedia_files
{
    TableRow CreateTableRow(const QueryResult &result)
    {
        if (result.getFieldCount() != magic_enum::enum_count<TableFields>() + 1) {
            return TableRow{};
        }

        return TableRow{
            result[0].getUInt32(),    // ID
            {result[1].getString(),   // path
             result[2].getString(),   // mediaType
             result[3].getUInt32()},  // size
            {result[4].getString(),   // title
             {result[5].getString(),  // artist
              result[6].getString()}, // album title
             result[7].getString(),   // comment
             result[8].getString(),   // genre
             result[9].getUInt32(),   // year
             result[10].getUInt32()}, // track
            {result[11].getUInt32(),  // songLength
             result[12].getUInt32(),  // bitrate
             result[13].getUInt32(),  // sample rate
             result[14].getUInt32()}, // channels
        };
    }

    auto TableRow::isValid() const -> bool
    {
        return (!fileInfo.path.empty() && Record::isValid());
    }

    MultimediaFilesTable::MultimediaFilesTable(Database *db) : Table(db)
    {
        createTableRow = CreateTableRow;
    }

    bool MultimediaFilesTable::create()
    {
        return true;
    }

    bool MultimediaFilesTable::add(TableRow entry)
    {
        return db->execute("INSERT INTO files (path, media_type, size, title, artist, album, "
                           "comment, genre, year, track, song_length, bitrate, sample_rate, channels) "
                           "VALUES('%q', '%q', %lu, '%q', '%q', '%q', '%q', '%q', %lu, %lu, %lu, %lu, %lu, %lu) "
                           "ON CONFLICT(path) DO UPDATE SET "
                           "path = excluded.path, "
                           "media_type = excluded.media_type, "
                           "size = excluded.size, "
                           "title = excluded.title, "
                           "artist = excluded.artist, "
                           "album = excluded.album, "
                           "comment = excluded.comment, "
                           "genre = excluded.genre, "
                           "year = excluded.year, "
                           "track = excluded.track, "
                           "song_length = excluded.song_length, "
                           "bitrate = excluded.bitrate, "
                           "sample_rate = excluded.sample_rate, "
                           "channels = excluded.channels;",
                           entry.fileInfo.path.c_str(),
                           entry.fileInfo.mediaType.c_str(),
                           entry.fileInfo.size,
                           entry.tags.title.c_str(),
                           entry.tags.album.artist.c_str(),
                           entry.tags.album.title.c_str(),
                           entry.tags.comment.c_str(),
                           entry.tags.genre.c_str(),
                           entry.tags.year,
                           entry.tags.track,
                           entry.audioProperties.songLength,
                           entry.audioProperties.bitrate,
                           entry.audioProperties.sampleRate,
                           entry.audioProperties.channels);
    }

    bool MultimediaFilesTable::removeById(uint32_t id)
    {
        return db->execute("DELETE FROM files WHERE _id = %lu;", id);
    }

    bool MultimediaFilesTable::removeByField(TableFields field, const char *str)
    {
        const auto &fieldName = getFieldName(field);

        if (fieldName.empty()) {
            return false;
        }

        return db->execute("DELETE FROM files WHERE %q = '%q';", fieldName.c_str(), str);
    }

    bool MultimediaFilesTable::removeAll()
    {
        return db->execute("DELETE FROM files;");
    }

    bool MultimediaFilesTable::update(TableRow entry)
    {
        return db->execute("UPDATE files SET path = '%q', media_type = '%q', size = %lu, title = '%q', artist = '%q',"
                           "album = '%q', comment = '%q', genre = '%q', year = %lu, track = %lu, song_length = %lu,"
                           "bitrate = %lu, sample_rate = %lu, channels = %lu WHERE _id = %lu;",
                           entry.fileInfo.path.c_str(),
                           entry.fileInfo.mediaType.c_str(),
                           entry.fileInfo.size,
                           entry.tags.title.c_str(),
                           entry.tags.album.artist.c_str(),
                           entry.tags.album.title.c_str(),
                           entry.tags.comment.c_str(),
                           entry.tags.genre.c_str(),
                           entry.tags.year,
                           entry.tags.track,
                           entry.audioProperties.songLength,
                           entry.audioProperties.bitrate,
                           entry.audioProperties.sampleRate,
                           entry.audioProperties.channels,
                           entry.ID);
    }

    bool MultimediaFilesTable::addOrUpdate(TableRow entry, std::string oldPath)
    {
        auto path = oldPath.empty() ? entry.fileInfo.path : oldPath;

        return db->execute("BEGIN TRANSACTION; "
                           "INSERT OR IGNORE INTO files (path) VALUES ('%q'); "
                           "UPDATE files SET path = '%q', media_type = '%q', size = %lu, title = '%q', artist = '%q', "
                           "album = '%q', comment = '%q', genre = '%q', year = %lu, track = %lu, song_length = %lu, "
                           "bitrate = %lu, sample_rate = %lu, channels = %lu WHERE path = '%q'; "
                           "COMMIT;",
                           path.c_str(),
                           entry.fileInfo.path.c_str(),
                           entry.fileInfo.mediaType.c_str(),
                           entry.fileInfo.size,
                           entry.tags.title.c_str(),
                           entry.tags.album.artist.c_str(),
                           entry.tags.album.title.c_str(),
                           entry.tags.comment.c_str(),
                           entry.tags.genre.c_str(),
                           entry.tags.year,
                           entry.tags.track,
                           entry.audioProperties.songLength,
                           entry.audioProperties.bitrate,
                           entry.audioProperties.sampleRate,
                           entry.audioProperties.channels,
                           path.c_str());
    }

    TableRow MultimediaFilesTable::getById(uint32_t id)
    {
        auto retQuery = db->query("SELECT * FROM files WHERE _id = %lu;", id);

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return TableRow();
        }

        return CreateTableRow(*retQuery);
    }

    TableRow MultimediaFilesTable::getByPath(std::string path)
    {
        auto retQuery = db->query("SELECT * FROM files WHERE path = '%q';", path.c_str());

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return TableRow();
        }

        return CreateTableRow(*retQuery);
    }

    std::vector<TableRow> MultimediaFilesTable::getLimitOffset(uint32_t offset, uint32_t limit)
    {
        auto retQuery = db->query("SELECT * from files ORDER BY title ASC LIMIT %lu OFFSET %lu;", limit, offset);

        return retQueryUnpack(std::move(retQuery));
    }

    auto MultimediaFilesTable::getArtistsLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<Artist>
    {
        auto retQuery =
            db->query("SELECT DISTINCT artist from files ORDER BY artist ASC LIMIT %lu OFFSET %lu;", limit, offset);

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return {};
        }

        std::vector<Artist> outVector;

        do {
            outVector.push_back((*retQuery)[0].getString()); // artist
        } while (retQuery->nextRow());
        return outVector;
    }

    std::vector<TableRow> MultimediaFilesTable::getLimitOffsetByField(uint32_t offset,
                                                                      uint32_t limit,
                                                                      TableFields field,
                                                                      const char *str)
    {
        std::unique_ptr<QueryResult> retQuery = nullptr;
        const auto &fieldName                 = getFieldName(field);

        if (fieldName.empty() || str == nullptr) {
            return {};
        }

        retQuery = db->query("SELECT * FROM files WHERE %q = '%q' ORDER BY title ASC LIMIT %lu OFFSET %lu;",
                             fieldName.c_str(),
                             str,
                             limit,
                             offset);

        return retQueryUnpack(std::move(retQuery));
    }

    uint32_t MultimediaFilesTable::count()
    {
        auto queryRet = db->query("SELECT COUNT(*) FROM files;");
        if (!queryRet || queryRet->getRowCount() == 0) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }

    uint32_t MultimediaFilesTable::countArtists()
    {
        auto queryRet = db->query("SELECT COUNT (DISTINCT artist) FROM files;");
        if (!queryRet || queryRet->getRowCount() == 0) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }

    auto MultimediaFilesTable::getAlbumsLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<Album>
    {
        auto retQuery = db->query(
            "SELECT DISTINCT artist,album from files ORDER BY album ASC LIMIT %lu OFFSET %lu;", limit, offset);

        if ((retQuery == nullptr) || (retQuery->getRowCount() < 2)) {
            return {};
        }

        std::vector<Album> outVector;

        do {
            outVector.push_back({.artist = (*retQuery)[0].getString(), .title = (*retQuery)[1].getString()});
        } while (retQuery->nextRow());
        return outVector;
    }

    uint32_t MultimediaFilesTable::countAlbums()
    {
        auto queryRet = db->query("SELECT COUNT(*) FROM"
                                  "(SELECT DISTINCT artist,album from files);");
        if (!queryRet || queryRet->getRowCount() == 0) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }

    uint32_t MultimediaFilesTable::countByFieldId(const char *field, uint32_t id)
    {
        if (field == nullptr) {
            return 0;
        }

        auto queryRet = db->query("SELECT COUNT(*) FROM files WHERE %q=%lu;", field, id);
        if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }

    std::string MultimediaFilesTable::getFieldName(TableFields field)
    {
        return utils::enumToString(field);
    }

    auto MultimediaFilesTable::getLimitOffset(const Artist &artist, uint32_t offset, uint32_t limit)
        -> std::vector<TableRow>
    {
        return getLimitOffsetByField(offset, limit, TableFields::artist, artist.c_str());
    }

    auto MultimediaFilesTable::count(const Artist &artist) -> uint32_t
    {
        auto queryRet = db->query("SELECT COUNT(*) FROM files WHERE artist='%q';", artist.c_str());
        if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }

    auto MultimediaFilesTable::getLimitOffset(const Album &album, uint32_t offset, uint32_t limit)
        -> std::vector<TableRow>
    {
        std::unique_ptr<QueryResult> retQuery = db->query(
            "SELECT * FROM files WHERE artist = '%q' AND album = '%q' ORDER BY title ASC LIMIT %lu OFFSET %lu;",
            album.artist.c_str(),
            album.title.c_str(),
            limit,
            offset);

        return retQueryUnpack(std::move(retQuery));
    }

    auto MultimediaFilesTable::count(const Album &album) -> uint32_t
    {
        auto queryRet = db->query("SELECT COUNT(*) FROM files WHERE artist='%q' AND album = '%q';",
                                  album.artist.c_str(),
                                  album.title.c_str());
        if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }
} // namespace db::multimedia_files
