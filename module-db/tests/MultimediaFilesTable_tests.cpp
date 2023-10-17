// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Helpers.hpp>
#include <databases/MultimediaFilesDB.hpp>
#include <Interface/MultimediaFilesRecord.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesAdd.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesEdit.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesGet.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesGetLimited.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesRemove.hpp>
#include <queries/multimedia_files/QueryMultimediaFilesCount.hpp>

#include <algorithm>
using namespace db::multimedia_files;

const std::vector<std::string> artists = {{""}, {"Just an artist"}, {"Mega artist"}, {"Super artist"}};

constexpr auto song1 = "Super song";
constexpr auto song2 = "Mega song";
constexpr auto song3 = "Just a song";

constexpr auto album1 = "Super album";
constexpr auto album2 = "Mega album";
constexpr auto album3 = "Just an album";

const std::vector<Album> albums = {{.artist = artists[0], .title = {}},
                                   {.artist = artists[0], .title = album1},
                                   {.artist = artists[1], .title = {}},
                                   {.artist = artists[1], .title = album1},
                                   {.artist = artists[1], .title = album2},
                                   {.artist = artists[2], .title = album1},
                                   {.artist = artists[2], .title = album3},
                                   {.artist = artists[3], .title = album2}};

const std::vector<TableRow> records = {
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file1.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song1, .album = albums[3], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file2.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song2, .album = albums[3], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file3.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song2, .album = albums[4], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file4.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song2, .album = albums[5], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file5.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[0], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file6.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[7], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/audio/file7.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[5], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/file1.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[2], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/file2.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[1], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .fileInfo        = {.path = "user/file3.mp3", .mediaType = "audio/mp3", .size = 100},
     .tags            = {.title = song3, .album = albums[6], .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}}};

TEST_CASE("Multimedia DB tests")
{
    db::tests::DatabaseUnderTest<MultimediaFilesDB> db{"multimedia.db", db::tests::getScriptsPath()};
    MultimediaFilesRecordInterface multimediaFilesRecordInterface(&db.get());

    constexpr auto PageSize = 8;

    SECTION("Table")
    {
        SECTION("TableRow")
        {
            auto record = TableRow{};
            REQUIRE(!record.isValid());

            record.ID            = 1;
            record.fileInfo.path = "audio";

            REQUIRE(record.isValid());
        }

        SECTION("Empty")
        {
            REQUIRE(db.get().files.count() == 0);
            REQUIRE(db.get().files.getLimitOffset(0, PageSize).empty());
        }

        SECTION("Add, get and remove")
        {
            const auto path = "audio/user";
            TableRow record;
            record.fileInfo.path = path;
            REQUIRE(!record.isValid());
            REQUIRE(db.get().files.add(record));

            REQUIRE(db.get().files.count() == 1);
            auto result = db.get().files.getById(1);
            REQUIRE(result.isValid());

            SECTION("Remove by ID")
            {
                REQUIRE(db.get().files.removeById(1));
                REQUIRE(db.get().files.count() == 0);
                auto result = db.get().files.getById(1);
                REQUIRE(!result.isValid());
            }
            SECTION("Remove by field")
            {
                REQUIRE(db.get().files.removeByField(TableFields::path, path));
                REQUIRE(db.get().files.count() == 0);
                auto result = db.get().files.getById(1);
                REQUIRE(!result.isValid());
            }
        }

        for (const auto &record : records) {
            REQUIRE(db.get().files.add(record));
        }

        SECTION("Remove all")
        {
            REQUIRE(db.get().files.count() == records.size());
            REQUIRE(db.get().files.removeAll());
            REQUIRE(db.get().files.count() == 0);
        }

        SECTION("Add for existing path")
        {
            auto resultPre               = db.get().files.getById(2);
            resultPre.fileInfo.mediaType = "bla bla";
            REQUIRE(db.get().files.add(resultPre));
            auto resultPost = db.get().files.getById(2);
            REQUIRE((resultPre.ID == resultPost.ID && resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
        }

        SECTION("Update")
        {
            auto resultPre               = db.get().files.getById(2);
            resultPre.fileInfo.mediaType = "bla bla";
            REQUIRE(db.get().files.update(resultPre));
            auto resultPost = db.get().files.getById(2);
            REQUIRE((resultPre.ID == resultPost.ID && resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
        }

        SECTION("Add or Update")
        {
            REQUIRE(db.get().files.removeAll());
            REQUIRE(db.get().files.count() == 0);

            for (const auto &record : records) {
                REQUIRE(db.get().files.addOrUpdate(record));
            }

            REQUIRE(db.get().files.count() == records.size());

            auto oldPath           = records[1].fileInfo.path;
            auto resultPre         = db.get().files.getByPath(oldPath);
            const auto referenceID = resultPre.ID;
            resultPre.ID           = 10;

            SECTION("No changes")
            {
                REQUIRE(db.get().files.addOrUpdate(resultPre));
                REQUIRE(db.get().files.count() == records.size());
                auto resultPost = db.get().files.getByPath(oldPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path));
            }
            SECTION("change path")
            {
                auto newPath            = "user/newPath";
                resultPre.fileInfo.path = newPath;
                REQUIRE(db.get().files.addOrUpdate(resultPre, oldPath));
                REQUIRE(db.get().files.count() == records.size());
                auto notExistingEntry = db.get().files.getByPath(oldPath);
                REQUIRE(!notExistingEntry.isValid());
                auto resultPost = db.get().files.getByPath(newPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path));
            }
            SECTION("change any field")
            {
                resultPre.fileInfo.mediaType = "bla bla";
                REQUIRE(db.get().files.addOrUpdate(resultPre));
                auto resultPost = db.get().files.getByPath(oldPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path &&
                         resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
            }
        }

        SECTION("getLimitOffset")
        {
            auto size = records.size();
            REQUIRE(db.get().files.getLimitOffset(0, size - 3).size() == size - 3);
            REQUIRE(db.get().files.getLimitOffset(size - 3, size).size() == 3);
        }

        SECTION("getLimitOffsetByField")
        {
            auto size = records.size();
            REQUIRE(db.get().files.getLimitOffsetByField(0, size, TableFields::artist, artists[1].c_str()).size() == 4);
        }

        SECTION("Artists")
        {
            REQUIRE(db.get().files.countArtists() == artists.size());
            auto artistsList = db.get().files.getArtistsLimitOffset(0, artists.size());
            REQUIRE(artistsList.size() == artists.size());
            for (size_t i = 0; i < artists.size(); i++) {
                REQUIRE(artistsList[i] == artists[i]);
            }

            SECTION("getLimitOffset")
            {
                const auto size         = artists.size();
                constexpr auto reminder = 2;
                const auto pageSize     = size - reminder;
                REQUIRE(db.get().files.getLimitOffset(0, pageSize).size() == pageSize);
                REQUIRE(db.get().files.getLimitOffset(pageSize, pageSize).size() == reminder);
            }
        }

        const auto numberOfAlbums = albums.size();
        SECTION("Albums")
        {
            REQUIRE(db.get().files.countAlbums() == numberOfAlbums);
            auto albumsList = db.get().files.getAlbumsLimitOffset(0, numberOfAlbums);
            REQUIRE(albumsList.size() == numberOfAlbums);
            for (const auto &album : albumsList) {
                auto it = std::find_if(albums.begin(), albums.end(), [album](const Album &albumRef) {
                    return album.artist == albumRef.artist && album.title == albumRef.title;
                });
                REQUIRE(it != albums.end());
            }

            SECTION("getLimitOffset")
            {
                const auto size         = numberOfAlbums;
                constexpr auto reminder = 2;
                const auto pageSize     = size - reminder;
                REQUIRE(db.get().files.getAlbumsLimitOffset(0, pageSize).size() == pageSize);
                REQUIRE(db.get().files.getAlbumsLimitOffset(pageSize, pageSize).size() == reminder);
            }
        }

        SECTION("Get songs for artist")
        {
            for (const auto &artist : artists) {
                size_t size = std::count_if(records.begin(), records.end(), [artist](const TableRow &record) {
                    return record.tags.album.artist == artist;
                });
                REQUIRE(db.get().files.count(artist) == size);
                auto artistsList = db.get().files.getLimitOffset(artist, 0, records.size());
                REQUIRE(artistsList.size() == size);
                for (size_t i = 0; i < artistsList.size(); i++) {
                    REQUIRE(artistsList[i].tags.album.artist == artist);
                }
            }
        }

        SECTION("Get songs from album")
        {
            for (const auto &album : albums) {
                size_t size = std::count_if(records.begin(), records.end(), [album](const TableRow &record) {
                    return record.tags.album.artist == album.artist && record.tags.album.title == album.title;
                });
                REQUIRE(db.get().files.count(album) == size);
                auto artistsList = db.get().files.getLimitOffset(album, 0, records.size());
                REQUIRE(artistsList.size() == size);
                for (size_t i = 0; i < artistsList.size(); i++) {
                    REQUIRE((artistsList[i].tags.album.artist == album.artist &&
                             artistsList[i].tags.album.title == album.title));
                }
            }
        }
    }

    SECTION("Queries")
    {
        auto addQuery = [&](const MultimediaFilesRecord &record) {
            auto query  = std::make_shared<db::multimedia_files::query::Add>(record);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::AddResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto addOrUpdateQuery = [&](const MultimediaFilesRecord &record, std::string oldPath = "") {
            auto query  = std::make_shared<db::multimedia_files::query::AddOrEdit>(record, oldPath);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::AddOrEditResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto getQuery = [&](uint32_t id) {
            auto query  = std::make_shared<db::multimedia_files::query::Get>(id);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getResult();
        };

        auto getByPathQuery = [&](std::string path) {
            auto query  = std::make_shared<db::multimedia_files::query::GetByPath>(path);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getResult();
        };

        auto getLimitedQuery = [&](const uint32_t offset, const uint32_t limit) {
            auto query  = std::make_shared<db::multimedia_files::query::GetLimited>(offset, limit);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            return record;
        };

        auto getLimitedByPathsQuery =
            [&](const std::vector<std::string> &paths, const uint32_t offset, const uint32_t limit) {
                auto query  = std::make_shared<db::multimedia_files::query::GetLimitedByPaths>(paths, offset, limit);
                auto ret    = multimediaFilesRecordInterface.runQuery(query);
                auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(ret.get());
                REQUIRE(result != nullptr);
                auto record = result->getResult();
                return record;
            };

        auto updateQuery = [&](const MultimediaFilesRecord &record) {
            const auto query  = std::make_shared<db::multimedia_files::query::Edit>(record);
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::EditResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto getCountQuery = [&]() {
            const auto query  = std::make_shared<db::multimedia_files::query::GetCount>();
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getCount();
        };

        auto removeQuery = [&](uint32_t id) {
            const auto query  = std::make_shared<db::multimedia_files::query::Remove>(id);
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::RemoveResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto removeByPathQuery = [&](const std::string &path) {
            const auto query  = std::make_shared<db::multimedia_files::query::RemoveByPath>(path);
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::RemoveResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto removeAllQuery = [&]() {
            const auto query  = std::make_shared<db::multimedia_files::query::RemoveAll>();
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::RemoveResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
        };

        auto getCountArtistsQuery = [&]() {
            const auto query  = std::make_shared<db::multimedia_files::query::GetCountArtists>();
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getCount();
        };

        auto getArtistsLimitedQuery = [&](const uint32_t offset, const uint32_t limit) {
            auto query  = std::make_shared<db::multimedia_files::query::GetArtistsLimited>(offset, limit);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetArtistsLimitedResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            return record;
        };

        auto getCountAlbumsQuery = [&]() {
            const auto query  = std::make_shared<db::multimedia_files::query::GetCountAlbums>();
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getCount();
        };

        auto getAlbumsLimitedQuery = [&](const uint32_t offset, const uint32_t limit) {
            auto query  = std::make_shared<db::multimedia_files::query::GetAlbumsLimited>(offset, limit);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetAlbumsLimitedResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            return record;
        };

        auto getLimitedQueryForArtist = [&](const Artist &artist, const uint32_t offset, const uint32_t limit) {
            auto query  = std::make_shared<db::multimedia_files::query::GetLimitedForArtist>(artist, offset, limit);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            return record;
        };

        auto getLimitedQueryForAlbum = [&](const Album &album, const uint32_t offset, const uint32_t limit) {
            auto query  = std::make_shared<db::multimedia_files::query::GetLimitedForAlbum>(album, offset, limit);
            auto ret    = multimediaFilesRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::multimedia_files::query::GetLimitedResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            return record;
        };

        auto getCountQueryForArtist = [&](const Artist &artist) {
            const auto query  = std::make_shared<db::multimedia_files::query::GetCountForArtist>(artist);
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getCount();
        };

        auto getCountQueryForAlbum = [&](const Album &album) {
            const auto query  = std::make_shared<db::multimedia_files::query::GetCountForAlbum>(album);
            const auto ret    = multimediaFilesRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::multimedia_files::query::GetCountResult *>(ret.get());
            REQUIRE(result != nullptr);
            return result->getCount();
        };

        SECTION("add, get, remove query")
        {
            const auto path = "audio/user";
            MultimediaFilesRecord record;
            record.fileInfo.path = path;
            REQUIRE(!record.isValid());
            addQuery(record);

            REQUIRE(getCountQuery() == 1);
            auto result = getQuery(1);
            REQUIRE(result.isValid());

            SECTION("Remove by ID")
            {
                removeQuery(1);
                REQUIRE(getCountQuery() == 0);
                auto result = getQuery(1);
                REQUIRE(!result.isValid());
            }
            SECTION("Remove by Path")
            {
                removeByPathQuery(path);
                REQUIRE(getCountQuery() == 0);
                auto result = getQuery(1);
                REQUIRE(!result.isValid());
            }
        }

        for (const auto &record : records) {
            addQuery(record);
        }

        SECTION("Remove all")
        {
            REQUIRE(getCountQuery() == records.size());
            removeAllQuery();
            REQUIRE(getCountQuery() == 0);
        }

        SECTION("Add for existing path")
        {
            auto resultPre               = getQuery(2);
            resultPre.fileInfo.mediaType = "bla bla";
            addQuery(resultPre);
            auto resultPost = getQuery(2);
            REQUIRE((resultPre.ID == resultPost.ID && resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
        }

        SECTION("Update")
        {
            auto resultPre               = getQuery(2);
            resultPre.fileInfo.mediaType = "bla bla";
            updateQuery(resultPre);
            auto resultPost = getQuery(2);
            REQUIRE((resultPre.ID == resultPost.ID && resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
        }

        SECTION("Add or Update")
        {
            removeAllQuery();
            REQUIRE(getCountQuery() == 0);

            for (const auto &record : records) {
                addOrUpdateQuery(record);
            }

            REQUIRE(getCountQuery() == records.size());

            auto oldPath           = records[1].fileInfo.path;
            auto resultPre         = getByPathQuery(oldPath);
            const auto referenceID = resultPre.ID;
            resultPre.ID           = 10;

            SECTION("No changes")
            {
                addOrUpdateQuery(resultPre);
                REQUIRE(getCountQuery() == records.size());
                auto resultPost = getByPathQuery(oldPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path));
            }
            SECTION("change path")
            {
                auto newPath            = "user/newPath";
                resultPre.fileInfo.path = newPath;
                addOrUpdateQuery(resultPre, oldPath);
                REQUIRE(getCountQuery() == records.size());
                auto notExistingEntry = getByPathQuery(oldPath);
                REQUIRE(!notExistingEntry.isValid());
                auto resultPost = getByPathQuery(newPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path));
            }
            SECTION("change any field")
            {
                resultPre.fileInfo.mediaType = "bla bla";
                addOrUpdateQuery(resultPre);
                auto resultPost = getByPathQuery(oldPath);
                REQUIRE(resultPost.isValid());
                REQUIRE((resultPost.ID == referenceID && resultPre.fileInfo.path == resultPost.fileInfo.path &&
                         resultPre.fileInfo.mediaType == resultPost.fileInfo.mediaType));
            }
        }

        SECTION("getLimitOffset")
        {
            auto size = records.size();
            REQUIRE(getLimitedQuery(0, size - 3).size() == size - 3);
            REQUIRE(getLimitedQuery(size - 3, size).size() == 3);
        }

        SECTION("getLimitOffsetByPaths")
        {
            MultimediaFilesRecord entry;
            const auto user_count       = records.size();
            const auto user_audio_count = user_count - 3;

            REQUIRE(getLimitedByPathsQuery({"user/"}, 0, UINT32_MAX).size() == user_count);
            REQUIRE(getLimitedByPathsQuery({"user/audio/"}, 0, UINT32_MAX).size() == user_audio_count);
            REQUIRE(getLimitedByPathsQuery({"user/audio1/"}, 0, UINT32_MAX).size() == 0);
            REQUIRE(getLimitedByPathsQuery({"user/audio2"}, 0, UINT32_MAX).size() == 0);
            REQUIRE(getLimitedByPathsQuery({"abc/"}, 0, UINT32_MAX).size() == 0);

            entry.fileInfo.path = "assets/audio/file30.mp3";
            addQuery(entry);
            entry.fileInfo.path = "assets/audio/file31.mp3";
            addQuery(entry);
            REQUIRE(getLimitedByPathsQuery({"user/audio", "assets/audio"}, 0, UINT32_MAX).size() ==
                    user_audio_count + 2);
            REQUIRE(getLimitedByPathsQuery({"user/audio", "assets/audio", "non/existing/path"}, 0, UINT32_MAX).size() ==
                    user_audio_count + 2);

            entry.fileInfo.path = "assets/audio/app/file32.mp3";
            addQuery(entry);
            REQUIRE(getLimitedByPathsQuery({"user/audio", "assets/audio/app"}, 0, UINT32_MAX).size() ==
                    user_audio_count + 1);
        }

        SECTION("Artists")
        {
            REQUIRE(getCountArtistsQuery() == artists.size());
            auto artistsList = getArtistsLimitedQuery(0, artists.size());
            REQUIRE(artistsList.size() == artists.size());
            for (size_t i = 0; i < artists.size(); i++) {
                REQUIRE(artistsList[i] == artists[i]);
            }

            SECTION("getLimitOffset")
            {
                const auto size         = artists.size();
                constexpr auto reminder = 2;
                const auto pageSize     = size - reminder;
                REQUIRE(getArtistsLimitedQuery(0, pageSize).size() == pageSize);
                REQUIRE(getArtistsLimitedQuery(pageSize, pageSize).size() == reminder);
            }
        }

        const auto numberOfAlbums = albums.size();
        SECTION("Albums")
        {
            REQUIRE(getCountAlbumsQuery() == numberOfAlbums);
            auto albumsList = getAlbumsLimitedQuery(0, numberOfAlbums);
            REQUIRE(albumsList.size() == numberOfAlbums);
            for (const auto &album : albumsList) {
                auto it = std::find_if(albums.begin(), albums.end(), [album](const Album &albumRef) {
                    return album.artist == albumRef.artist && album.title == albumRef.title;
                });
                REQUIRE(it != albums.end());
            }

            SECTION("getLimitOffset")
            {
                const auto size         = numberOfAlbums;
                constexpr auto reminder = 2;
                const auto pageSize     = size - reminder;
                REQUIRE(db.get().files.getAlbumsLimitOffset(0, pageSize).size() == pageSize);
                REQUIRE(db.get().files.getAlbumsLimitOffset(pageSize, pageSize).size() == reminder);
            }
        }

        SECTION("Get songs for artist")
        {
            for (const auto &artist : artists) {
                size_t size = std::count_if(records.begin(), records.end(), [artist](const TableRow &record) {
                    return record.tags.album.artist == artist;
                });
                REQUIRE(getCountQueryForArtist(artist) == size);
                auto artistsList = getLimitedQueryForArtist(artist, 0, records.size());
                REQUIRE(artistsList.size() == size);
                for (size_t i = 0; i < artistsList.size(); i++) {
                    REQUIRE(artistsList[i].tags.album.artist == artist);
                }
            }
        }

        SECTION("Get songs from album")
        {
            for (const auto &album : albums) {
                size_t size = std::count_if(records.begin(), records.end(), [album](const TableRow &record) {
                    return record.tags.album.artist == album.artist && record.tags.album.title == album.title;
                });
                REQUIRE(getCountQueryForAlbum(album) == size);
                auto artistsList = getLimitedQueryForAlbum(album, 0, records.size());
                REQUIRE(artistsList.size() == size);
                for (size_t i = 0; i < artistsList.size(); i++) {
                    REQUIRE((artistsList[i].tags.album.artist == album.artist &&
                             artistsList[i].tags.album.title == album.title));
                }
            }
        }
    }
}
