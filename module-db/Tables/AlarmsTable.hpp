// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Record.hpp"
#include "Table.hpp"
#include <time/dateCommon.hpp>
#include <utf8/UTF8.hpp>

struct AlarmsRecord;

enum class AlarmStatus
{
    Off,
    On,
    FirstSnooze,
    SecondSnooze,
    ThirdSnooze,
    FourthSnooze,
    FifthSnooze
};

struct AlarmsTableRow : public Record
{
    TimePoint time     = TIME_POINT_INVALID;
    uint32_t snooze    = 0;
    AlarmStatus status = AlarmStatus::On;
    uint32_t repeat    = 0;
    UTF8 path;

    AlarmsTableRow() = default;
    AlarmsTableRow(uint32_t id, TimePoint time, uint32_t snooze, AlarmStatus status, uint32_t repeat, UTF8 path);
    explicit AlarmsTableRow(const AlarmsRecord &rec);
    explicit AlarmsTableRow(const QueryResult &result);
};

enum class AlarmsTableFields
{
    Time,
    Snooze,
    Status,
    Path
};

class AlarmsTable : public Table<AlarmsTableRow, AlarmsTableFields>
{
  public:
    AlarmsTable(Database *db);
    virtual ~AlarmsTable() = default;

    bool create() override final;
    bool add(AlarmsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(AlarmsTableFields field, const char *str) override final;
    bool update(AlarmsTableRow entry) override final;
    AlarmsTableRow getById(uint32_t id) override final;
    std::vector<AlarmsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<AlarmsTableRow> getLimitOffsetByField(uint32_t offset,
                                                      uint32_t limit,
                                                      AlarmsTableFields field,
                                                      const char *str) override final;

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    bool updateStatuses(AlarmStatus status);
};
