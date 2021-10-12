// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventsTable.hpp"

#include <Common/Common.hpp>
#include <Database/Database.hpp>

#include <utf8/UTF8.hpp>

struct AlarmEventRecord;

struct AlarmEventsTableRow : public Record
{
    uint32_t hourOfDay{0};
    uint32_t minuteOfHour{0};
    std::string musicTone{""};
    bool enabled{false};
    uint32_t snoozeDuration{0};
    std::string rruleText{""};

    AlarmEventsTableRow() = default;
    AlarmEventsTableRow(uint32_t id,
                        uint32_t hourOfDay,
                        uint32_t minuteOfHour,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration,
                        const std::string &rruleText);
    explicit AlarmEventsTableRow(const AlarmEventRecord &rec);
    explicit AlarmEventsTableRow(const QueryResult &result);

    auto isValid() const -> bool;
};

enum class AlarmEventsTableFields
{
    AlarmTimeHour,
    AlarmTimeMinute,
    MusicTone,
    Enabled,
    SnoozeDuration,
    Rrule,
};

class AlarmEventsTable : public Table<AlarmEventsTableRow, AlarmEventsTableFields>
{
  public:
    explicit AlarmEventsTable(Database *db);
    virtual ~AlarmEventsTable() = default;

    auto create() -> bool override;
    auto add(AlarmEventsTableRow entry) -> bool override;
    auto removeById(uint32_t id) -> bool override;
    auto update(AlarmEventsTableRow entry) -> bool override;
    auto getById(uint32_t id) -> AlarmEventsTableRow override;
    auto getLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<AlarmEventsTableRow> override;
    auto getLimitOffsetByField(uint32_t offset, uint32_t limit, AlarmEventsTableFields field, const char *str)
        -> std::vector<AlarmEventsTableRow> override;
    auto getEnabled() -> std::vector<AlarmEventsTableRow>;
    auto toggleAll(bool toggle) -> bool;

    auto count() -> uint32_t override;
    auto countByFieldId(const char *field, uint32_t id) -> uint32_t override;

  private:
    auto getFieldName(AlarmEventsTableFields field) -> std::string;
};
