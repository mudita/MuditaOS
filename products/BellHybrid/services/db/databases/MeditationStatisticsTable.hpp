// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Tables/Record.hpp>
#include <module-db/Tables/Table.hpp>
#include <module-db/Database/Database.hpp>

namespace db::meditation_stats
{
    struct TableRow : public Record
    {
        time_t timestamp;
        std::chrono::minutes duration;
    };

    enum class TableFields
    {
        timestamp,
        duration
    };

    class MeditationStatsTable : public Table<TableRow, TableFields>
    {
      public:
        explicit MeditationStatsTable(Database *db);
        virtual ~MeditationStatsTable() = default;

        auto create() -> bool override;
        auto add(TableRow entry) -> bool override;
        auto removeById(uint32_t id) -> bool override;
        auto removeByField(TableFields field, const char *str) -> bool override;
        auto update(TableRow entry) -> bool override;
        auto getById(uint32_t id) -> TableRow override;
        auto getLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<TableRow> override;
        auto getLimitOffsetByField(uint32_t offset, uint32_t limit, TableFields field, const char *str)
            -> std::vector<TableRow> override;
        auto count() -> uint32_t override;
        auto countByFieldId(const char *field, uint32_t id) -> uint32_t override;

        auto getByDays(std::uint32_t days) -> std::vector<TableRow>;
    };
} // namespace db::meditation_stats
