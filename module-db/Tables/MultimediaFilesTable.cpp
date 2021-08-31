// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultimediaFilesTable.hpp"

#include <Interface/AlarmEventRecord.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

namespace
{
    MultimediaFilesTableRow CreateMultimediaFilesTableRow(const QueryResult &result)
    {
        if (result.getFieldCount() != magic_enum::enum_count<MultimediaFilesTableFields>() + 1) {
            return MultimediaFilesTableRow{};
        }

        return MultimediaFilesTableRow{
            result[0].getUInt32(),    // ID
            result[1].getString(),    // path
            result[2].getString(),    // mediaType
            result[3].getUInt32(),    // size
            {result[4].getString(),   // title
             result[5].getString(),   // artist
             result[6].getString(),   // album
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

} // namespace

auto MultimediaFilesTableRow::isValid() const -> bool
{
    return (!path.empty() && Record::isValid());
}

MultimediaFilesTable::MultimediaFilesTable(Database *db) : Table(db)
{
    createTableRow = CreateMultimediaFilesTableRow;
}

bool MultimediaFilesTable::create()
{
    return true;
}

bool MultimediaFilesTable::add(MultimediaFilesTableRow entry)
{
    return db->execute("INSERT or ignore INTO files (path, media_type, size, title, artist, album,"
                       "comment, genre, year, track, song_length, bitrate, sample_rate, channels)"
                       "VALUES('%q', '%q', %lu, '%q', '%q', '%q', '%q', '%q', %lu, %lu, %lu, %lu, %lu, %lu);",
                       entry.path.c_str(),
                       entry.mediaType.c_str(),
                       entry.size,
                       entry.tags.title.c_str(),
                       entry.tags.artist.c_str(),
                       entry.tags.album.c_str(),
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

bool MultimediaFilesTable::removeByField(MultimediaFilesTableFields field, const char *str)
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

bool MultimediaFilesTable::update(MultimediaFilesTableRow entry)
{
    return db->execute("UPDATE files SET path = '%q', media_type = '%q', size = %lu, title = '%q', artist = '%q',"
                       "album = '%q', comment = '%q', genre = '%q', year = %lu, track = %lu, song_length = %lu,"
                       "bitrate = %lu, sample_rate = %lu, channels = %lu WHERE _id = %lu;",
                       entry.path.c_str(),
                       entry.mediaType.c_str(),
                       entry.size,
                       entry.tags.title.c_str(),
                       entry.tags.artist.c_str(),
                       entry.tags.album.c_str(),
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

MultimediaFilesTableRow MultimediaFilesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM files WHERE _id = %lu;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return MultimediaFilesTableRow();
    }

    return CreateMultimediaFilesTableRow(*retQuery);
}

std::vector<MultimediaFilesTableRow> MultimediaFilesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from files ORDER BY title ASC LIMIT %lu OFFSET %lu;", limit, offset);

    return retQueryUnpack(std::move(retQuery));
}

std::vector<MultimediaFilesTableRow> MultimediaFilesTable::getLimitOffsetByField(uint32_t offset,
                                                                                 uint32_t limit,
                                                                                 MultimediaFilesTableFields field,
                                                                                 const char *str)
{
    std::unique_ptr<QueryResult> retQuery = nullptr;
    const auto &fieldName                 = getFieldName(field);

    if (fieldName.empty()) {
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

uint32_t MultimediaFilesTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM files WHERE %q=%lu;", field, id);
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

std::string MultimediaFilesTable::getFieldName(MultimediaFilesTableFields field)
{
    return utils::enumToString(field);
}
