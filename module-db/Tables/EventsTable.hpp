// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include <module-db/Database/Database.hpp>
#include <module-db/Common/Common.hpp>
#include <utf8/UTF8.hpp>
#include <module-apps/application-calendar/data/dateCommon.hpp>

struct EventsTableRow : public Record
{
    std::string UID;
    std::string title;
    TimePoint date_from      = TIME_POINT_INVALID;
    TimePoint date_till      = TIME_POINT_INVALID;
    uint32_t reminder        = 0;
    uint32_t repeat          = 0;
    TimePoint reminder_fired = TIME_POINT_INVALID;
    std::string provider_type;
    std::string provider_id;
    std::string provider_iCalUid;
};

enum class EventsTableFields
{
    date_from,
    date_till
};

class EventsTable : public Table<EventsTableRow, EventsTableFields>
{
  public:
    EventsTable(Database *db);
    ~EventsTable() override = default;

    bool create() override final;
    bool add(EventsTableRow entry) override final;
    bool addDaily(EventsTableRow entry);
    bool addWeekly(EventsTableRow entry);
    bool addTwoWeeks(EventsTableRow entry);
    bool addMonth(EventsTableRow entry);
    bool addYear(EventsTableRow entry);
    std::vector<bool> parseOptions(uint32_t repeatOptionValue);
    bool addCustom(EventsTableRow entry);
    bool removeById(uint32_t id) override final;
    bool removeByUID(const std::string &UID);
    bool removeByField(EventsTableFields field, const char *str) override final;
    bool update(EventsTableRow entry) override final;
    bool updateByUID(EventsTableRow entry);
    bool drop();
    EventsTableRow getByUID(const std::string &UID);
    EventsTableRow getById(uint32_t id) override final;
    std::vector<EventsTableRow> selectByDatePeriod(TimePoint filter_from,
                                                   TimePoint filter_till,
                                                   uint32_t offset,
                                                   uint32_t limit);

    std::vector<EventsTableRow> selectByDate(TimePoint date, uint32_t offset, uint32_t limit);

    uint32_t count() override final;
    uint32_t countFromFilter(TimePoint from, TimePoint till);
    uint32_t countFromDayFilter(TimePoint filter);
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    std::vector<EventsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<EventsTableRow> getLimitOffsetByField(uint32_t offset,
                                                      uint32_t limit,
                                                      EventsTableFields field,
                                                      const char *str) override final;

    std::vector<EventsTableRow> getLimitOffsetByDate(uint32_t offset, uint32_t limit);
    std::vector<EventsTableRow> SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS events("
                                   "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "uid TEXT,"
                                   "title TEXT,"
                                   "date_from DATETIME,"
                                   "date_till DATETIME,"
                                   "reminder INTEGER,"
                                   "repeat INTEGER,"
                                   "reminder_fired DATETIME,"
                                   "provider_type TEXT,"
                                   "provider_id TEXT,"
                                   "provider_iCalUid TEXT,"
                                   "UNIQUE (title, date_from, date_till));";
};
