// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Databases/MultimediaFilesDB.hpp>

constexpr auto artist1 = "Super artist";
constexpr auto artist2 = "Mega artist";
constexpr auto artist3 = "Just an artist";

constexpr auto song1 = "Super song";
constexpr auto song2 = "Mega song";
constexpr auto song3 = "Just a song";

constexpr auto album1 = "Super album";
constexpr auto album2 = "Mega album";
constexpr auto album3 = "Just an album";

const MultimediaFilesTableRow records[] = {
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song1, .artist = artist1, .album = album1, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song2, .artist = artist1, .album = album1, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song2, .artist = artist1, .album = album2, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song2, .artist = artist2, .album = album1, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song3, .artist = artist3, .album = album2, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}},
    {Record{DB_ID_NONE},
     .path      = "user/music",
     .mediaType = "audio/mp3",
     .size      = 100,
     .tags = {.title = song3, .artist = artist2, .album = album3, .comment = "", .genre = "", .year = 2011, .track = 1},
     .audioProperties = {.songLength = 300, .bitrate = 320, .sampleRate = 44100, .channels = 1}}};

TEST_CASE("Multimedia DB tests")
{
    REQUIRE(Database::initialize());

    const auto path = (std::filesystem::path{"sys/user"} / "multimedia.db");
    if (std::filesystem::exists(path)) {
        REQUIRE(std::filesystem::remove(path));
    }

    MultimediaFilesDB db(path.c_str());
    REQUIRE(db.isInitialized());

    constexpr auto PageSize = 8;

    SECTION("MultimediaFilesTableRow")
    {
        auto record = MultimediaFilesTableRow{};
        REQUIRE(!record.isValid());

        record.ID   = 1;
        record.path = "music";

        REQUIRE(record.isValid());
    }

    SECTION("Empty")
    {
        REQUIRE(db.files.count() == 0);
        REQUIRE(db.files.getLimitOffset(0, PageSize).empty());
    }

    SECTION("Add, get and remove")
    {
        const auto path = "music/user";
        MultimediaFilesTableRow record;
        record.path = path;
        REQUIRE(!record.isValid());
        REQUIRE(db.files.add(record));

        REQUIRE(db.files.count() == 1);
        auto result = db.files.getById(1);
        REQUIRE(result.isValid());

        SECTION("Remove by ID")
        {
            REQUIRE(db.files.removeById(1));
            REQUIRE(db.files.count() == 0);
            auto result = db.files.getById(1);
            REQUIRE(!result.isValid());
        }
        SECTION("Remove by field")
        {
            REQUIRE(db.files.removeByField(MultimediaFilesTableFields::path, path));
            REQUIRE(db.files.count() == 0);
            auto result = db.files.getById(1);
            REQUIRE(!result.isValid());
        }
    }

    for (const auto &record : records) {
        REQUIRE(db.files.add(record));
    }

    SECTION("Remove all")
    {
        REQUIRE(db.files.count() == 6);
        REQUIRE(db.files.removeAll());
        REQUIRE(db.files.count() == 0);
    }

    SECTION("Update")
    {
        auto resultPre      = db.files.getById(2);
        resultPre.mediaType = "bla bla";
        REQUIRE(db.files.update(resultPre));
        auto resultPost = db.files.getById(2);
        REQUIRE((resultPre.ID == resultPost.ID && resultPre.mediaType == resultPost.mediaType));
    }

    SECTION("getLimitOffset")
    {
        REQUIRE(db.files.getLimitOffset(0, 4).size() == 4);
        REQUIRE(db.files.getLimitOffset(4, 4).size() == 2);
    }

    SECTION("getLimitOffsetByField")
    {
        REQUIRE(db.files.getLimitOffsetByField(0, 4, MultimediaFilesTableFields::artist, artist1).size() == 3);
    }

    REQUIRE(Database::deinitialize());
}
