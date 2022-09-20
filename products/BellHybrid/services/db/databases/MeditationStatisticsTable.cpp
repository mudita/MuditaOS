// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationStatisticsTable.hpp"
#include <Common/Types.hpp>
#include <time.h>
#include <chrono>
#include <date/date.h>

namespace
{
    using namespace db::meditation_stats;
    constexpr auto tableName = "meditation_stats";

    /// We could use the SQLite function "datetime(timestamp,'unixepoch') to generate the correct time string.
    /// Unfortunately, SQLite v3.38.1 has some problems converting UNIX timestamps. That's why we decided we would store
    /// timestamps using string format, as it doesn't cause any issues.

    std::string prepare_timestamp(time_t unix_timestamp)
    {
        char time_buf[64]{};
        struct tm ts
        {};
        ts = *gmtime(&unix_timestamp);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &ts);

        return time_buf;
    }

    time_t to_unixepoch_timestamp(std::string_view str)
    {
        std::istringstream in{str.data()};
        date::sys_seconds tp;
        in >> date::parse("%Y-%m-%d %H:%M:%S", tp);
        return tp.time_since_epoch().count();
    }

    std::vector<TableRow> get_by_x(Database *db, const std::string_view modifier, const std::uint32_t x)
    {
        if (x == 0) {
            return {};
        }

        const auto retQuery = db->query("SELECT * from " str_ " where timestamp BETWEEN "
                                        "datetime('now','-" u32_ " %s') and datetime('now');",
                                        tableName,
                                        x,
                                        modifier.data());

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return {};
        }

        std::vector<TableRow> ret;
        ret.reserve(retQuery->getRowCount());

        do {
            ret.push_back(TableRow{
                {(*retQuery)[0].getUInt32()},                       // ID
                to_unixepoch_timestamp((*retQuery)[1].getString()), // timestamp
                std::chrono::minutes{(*retQuery)[2].getUInt32()}    // duration
            });
        } while (retQuery->nextRow());

        return ret;
    }
} // namespace

namespace db::meditation_stats
{
    MeditationStatsTable::MeditationStatsTable(Database *db) : Table(db)
    {}

    auto MeditationStatsTable::create() -> bool
    {
        return true;
    }
    auto MeditationStatsTable::removeById([[maybe_unused]] uint32_t id) -> bool
    {
        return false;
    }
    auto MeditationStatsTable::removeByField([[maybe_unused]] const TableFields field, [[maybe_unused]] const char *str)
        -> bool
    {
        return false;
    }
    auto MeditationStatsTable::update([[maybe_unused]] const TableRow row) -> bool
    {
        return false;
    }
    auto MeditationStatsTable::getLimitOffsetByField([[maybe_unused]] const uint32_t offset,
                                                     [[maybe_unused]] const uint32_t limit,
                                                     [[maybe_unused]] const TableFields field,
                                                     [[maybe_unused]] const char *str) -> std::vector<TableRow>
    {
        return {};
    }
    auto MeditationStatsTable::countByFieldId([[maybe_unused]] const char *field, [[maybe_unused]] const uint32_t id)
        -> uint32_t
    {
        return 0;
    }
    auto MeditationStatsTable::getById([[maybe_unused]] const uint32_t id) -> TableRow
    {
        return {};
    }

    auto MeditationStatsTable::add(const TableRow entry) -> bool
    {
        return db->execute("INSERT INTO " str_ " (timestamp,duration) "
                           "VALUES(" str_c u32_ ")",
                           tableName,
                           prepare_timestamp(entry.timestamp).c_str(),
                           static_cast<std::uint32_t>(entry.duration.count()));
    }
    auto MeditationStatsTable::getLimitOffset(const uint32_t offset, const uint32_t limit) -> std::vector<TableRow>
    {
        const auto retQuery = db->query(
            "SELECT * from " str_ " ORDER BY timestamp DESC LIMIT " u32_ " OFFSET " u32_ ";", tableName, limit, offset);

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return {};
        }

        std::vector<TableRow> ret;
        ret.reserve(retQuery->getRowCount());

        do {
            ret.push_back(TableRow{
                (*retQuery)[0].getUInt32(),                         // ID
                to_unixepoch_timestamp((*retQuery)[1].getString()), // timestamp
                std::chrono::minutes{(*retQuery)[2].getUInt32()}    // duration
            });
        } while (retQuery->nextRow());

        return ret;
    }
    auto MeditationStatsTable::count() -> uint32_t
    {
        const auto queryRet = db->query("SELECT COUNT(*) FROM" str_ ";", tableName);
        if (queryRet == nullptr || queryRet->getRowCount() == 0) {
            return 0;
        }

        return (*queryRet)[0].getUInt32();
    }
    auto MeditationStatsTable::getByDays(const uint32_t days) -> std::vector<TableRow>
    {
        return get_by_x(db, "days", days);
    }
} // namespace db::meditation_stats