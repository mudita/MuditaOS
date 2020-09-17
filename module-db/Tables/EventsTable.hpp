#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include <Database/Database.hpp>
#include <Common/Common.hpp>
#include <utf8/UTF8.hpp>
#include <module-apps/application-calendar/data/dateCommon.hpp>

struct EventsTableRow : public Record
{
    std::string title;
    TimePoint date_from = TIME_POINT_INVALID;
    TimePoint date_till = TIME_POINT_INVALID;
    uint32_t reminder  = 0;
    uint32_t repeat    = 0;
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
    bool addCustom(EventsTableRow entry);
    bool removeById(uint32_t id) override final;
    bool removeByField(EventsTableFields field, const char *str) override final;
    bool update(EventsTableRow entry) override final;
    EventsTableRow getById(uint32_t id) override final;
    std::vector<EventsTableRow> selectByDatePeriod(TimePoint filter_from, TimePoint filter_till);
    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    std::vector<EventsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<EventsTableRow> getLimitOffsetByField(uint32_t offset,
                                                      uint32_t limit,
                                                      EventsTableFields field,
                                                      const char *str) override final;

    std::vector<EventsTableRow> getLimitOffsetByDate(uint32_t offset, uint32_t limit);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS events("
                                   "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "title TEXT,"
                                   "date_from DATETIME,"
                                   "date_till DATETIME,"
                                   "reminder INTEGER,"
                                   "repeat INTEGER);";
};
