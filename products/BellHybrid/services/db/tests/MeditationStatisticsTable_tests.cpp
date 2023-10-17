// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <MeditationStatisticsDB.hpp>
#include <Helpers.hpp>

#include <filesystem>
#include <iostream>

namespace
{
    using namespace db::meditation_stats;

    time_t subtract_time(std::chrono::minutes minutes)
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - minutes);
    }

    time_t get_utc_time()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::filesystem::path getScriptsPath()
    {
        const std::string path = __FILE__;
        const auto dir         = std::filesystem::path{path.substr(0, path.rfind('/'))};
        return dir / "../databases/migration";
    }
} // namespace

TEST_CASE("Meditation statistics Table - API basic checks")
{
    db::tests::DatabaseUnderTest<MeditationStatisticsDB> db{"meditation_stats.db", getScriptsPath()};

    const auto timestamp = get_utc_time();
    REQUIRE(db.get().table.add({Record{0}, timestamp, std::chrono::minutes{20}}));
    REQUIRE(db.get().table.add({Record{0}, timestamp, std::chrono::minutes{10}}));
    REQUIRE(db.get().table.count() == 2);

    const auto query_result = db.get().table.getByDays(1);
    REQUIRE(query_result.size() == 2);
    REQUIRE(query_result[0].duration == std::chrono::minutes{20});
    REQUIRE(query_result[1].duration == std::chrono::minutes{10});
    REQUIRE(query_result[0].timestamp == timestamp);
    REQUIRE(query_result[1].timestamp == timestamp);

    REQUIRE(db.get().table.getLimitOffset(0, 3).size() == 2);
    REQUIRE(db.get().table.getLimitOffset(0, 1).size() == 1);
    REQUIRE(db.get().table.getLimitOffset(1, 1).size() == 1);

    /// Not implemented as it does not make sense to have such calls for meditation statistics table.
    REQUIRE(db.get().table.create());
    REQUIRE(not db.get().table.removeById(1));
    REQUIRE(not db.get().table.removeByField(TableFields::duration, "10"));
    REQUIRE(not db.get().table.getById(1).isValid());
    REQUIRE(db.get().table.countByFieldId("duration", 1) == 0);
    REQUIRE(db.get().table.getLimitOffsetByField(0, 2, TableFields::duration, "10").empty());
}

TEST_CASE("Meditation statistics Table - get by days")
{
    db::tests::DatabaseUnderTest<MeditationStatisticsDB> db{"meditation_stats.db", getScriptsPath()};

    REQUIRE(db.get().table.add({Record{0}, subtract_time(std::chrono::hours{23}), std::chrono::minutes{1}}));
    REQUIRE(db.get().table.getByDays(1).size() == 1);

    REQUIRE(db.get().table.add({Record{0}, subtract_time(std::chrono::hours{23}), std::chrono::minutes{2}}));
    REQUIRE(db.get().table.getByDays(1).size() == 2);

    REQUIRE(db.get().table.add({Record{0}, subtract_time(std::chrono::hours{24}), std::chrono::minutes{3}}));
    REQUIRE(db.get().table.add({Record{0}, subtract_time(std::chrono::hours{25}), std::chrono::minutes{4}}));
    REQUIRE(db.get().table.getByDays(1).size() == 3);
    REQUIRE(db.get().table.getByDays(2).size() == 4);
}
