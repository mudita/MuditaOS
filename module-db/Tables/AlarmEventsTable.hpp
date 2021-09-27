// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventsTable.hpp"

#include <Common/Common.hpp>
#include <Database/Database.hpp>

#include <utf8/UTF8.hpp>

struct AlarmEventRecord;

struct AlarmEventsTableRow : public EventsTableRow
{
    std::string musicTone{""};
    bool enabled{false};
    uint32_t snoozeDuration{0};

    AlarmEventsTableRow() = default;
    AlarmEventsTableRow(uint32_t id,
                        const UTF8 &name,
                        TimePoint startDate,
                        TimePoint endDate,
                        uint32_t duration,
                        bool isAllDay,
                        const std::string &rrule,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration);
    explicit AlarmEventsTableRow(const AlarmEventRecord &rec);
    explicit AlarmEventsTableRow(const QueryResult &result);

    auto isValid() const -> bool;
};

enum class AlarmEventsTableFields
{
    Name,
    StartDate,
    EndDate,
    Duration,
    IsAllDay,
    Rrule,
    MusicTone,
    Enabled,
    SnoozeDuration,
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
    auto getBetweenDates(TimePoint startDate, TimePoint endDate, std::uint32_t offset, std::uint32_t limit)
        -> std::pair<std::vector<AlarmEventsTableRow>, std::uint32_t>;
    auto getRecurringBetweenDates(TimePoint startDate, TimePoint endDate, uint32_t offset, uint32_t limit)
        -> std::vector<AlarmEventsTableRow>;
    auto getNext(TimePoint start, uint32_t offset, uint32_t limit) -> std::vector<AlarmEventsTableRow>;
    auto toggleAll(bool toggle) -> bool;

    auto count() -> uint32_t override;
    auto countByFieldId(const char *field, uint32_t id) -> uint32_t override;

  private:
    auto getFieldName(AlarmEventsTableFields field) -> std::string;
};
