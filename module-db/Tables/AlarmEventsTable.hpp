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
                        UTF8 name,
                        TimePoint startDate,
                        TimePoint endDate,
                        uint32_t duration,
                        bool isAllDay,
                        std::string rrule,
                        std::string musicTone,
                        bool enabled,
                        uint32_t snoozeDuration);
    explicit AlarmEventsTableRow(const AlarmEventRecord &rec);
    explicit AlarmEventsTableRow(const QueryResult &result);
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
    AlarmEventsTable(Database *db);
    virtual ~AlarmEventsTable() = default;

    bool create() override final;
    bool add(AlarmEventsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(AlarmEventsTableFields field, const char *str) override final;
    bool update(AlarmEventsTableRow entry) override final;
    AlarmEventsTableRow getById(uint32_t id) override final;
    std::vector<AlarmEventsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<AlarmEventsTableRow> getLimitOffsetByField(uint32_t offset,
                                                           uint32_t limit,
                                                           AlarmEventsTableFields field,
                                                           const char *str) override final;
    std::vector<AlarmEventsTableRow> getBetweenDates(TimePoint startDate,
                                                     TimePoint endDate,
                                                     uint32_t offset,
                                                     uint32_t limit);
    std::vector<AlarmEventsTableRow> getRecurringBetweenDates(TimePoint startDate,
                                                              TimePoint endDate,
                                                              uint32_t offset,
                                                              uint32_t limit);
    std::vector<AlarmEventsTableRow> getNext(TimePoint start, uint32_t offset, uint32_t limit);

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

  private:
    std::string getFieldName(AlarmEventsTableFields field);
};
