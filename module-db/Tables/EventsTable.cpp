#include "EventsTable.hpp"
#include "module-db/Interface/EventsRecord.hpp"

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>

EventsTable::EventsTable(Database *db) : Table(db)
{}

bool EventsTable::create()
{
    if (!db->execute(createTableQuery)) {
        return false;
    }

    return true;
}

bool EventsTable::add(EventsTableRow entry)
{
    return db->execute(
        "INSERT or IGNORE INTO events (title, description, date_from, date_till, reminder, repeat, time_zone) "
        "VALUES ('%q','%q', %lu, %lu, %lu, %lu, %lu);",
        entry.title.c_str(),
        entry.description.c_str(),
        entry.date_from,
        entry.date_till,
        entry.reminder,
        entry.repeat,
        entry.time_zone);
}

bool EventsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM events where _id = %u;", id);
}

bool EventsTable::removeByField(EventsTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case EventsTableFields::date_from: {
        fieldName = "date_from";
        break;
    }
    case EventsTableFields::date_till: {
        fieldName = "date_till";
        break;
    }
    }

    return db->execute("DELETE FROM events where %q = '%q';", fieldName.c_str(), str);
}

bool EventsTable::update(EventsTableRow entry)
{
    return db->execute("UPDATE events SET title= '%q', description = '%q', date_from = %u, date_till = %u, reminder "
                       "= %u, repeat = %u, time_zone = %u WHERE _id = %u;",
                       entry.title.c_str(),
                       entry.description.c_str(),
                       entry.date_from,
                       entry.date_till,
                       entry.reminder,
                       entry.repeat,
                       entry.time_zone,
                       entry.ID);
}

EventsTableRow EventsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM events WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return EventsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return EventsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getString(), // title
        (*retQuery)[2].getString(), // description
        (*retQuery)[3].getUInt32(), // date_from
        (*retQuery)[4].getUInt32(), // date_till
        (*retQuery)[5].getUInt32(), // reminder
        (*retQuery)[6].getUInt32(), // repeat
        (*retQuery)[7].getUInt32()  // time_zone

    };
}

std::vector<EventsTableRow> EventsTable::selectByDatePeriod(uint32_t date_from, uint32_t date_till)
{
    auto retQuery = db->query("SELECT * FROM events WHERE date_from >= %u AND date_till <= %u;", date_from, date_till);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getString(), // title
            (*retQuery)[2].getString(), // description
            (*retQuery)[3].getUInt32(), // date_from
            (*retQuery)[4].getUInt32(), // date_till
            (*retQuery)[5].getUInt32(), // reminder
            (*retQuery)[6].getUInt32(), // repeat
            (*retQuery)[7].getUInt32()  // time_zone
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from events LIMIT %lu OFFSET %lu;", limit, offset);
    //"SELECT * FROM event WHERE id = "

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getString(), // title
            (*retQuery)[2].getString(), // description
            (*retQuery)[3].getUInt32(), // date_from
            (*retQuery)[4].getUInt32(), // date_till
            (*retQuery)[5].getUInt32(), // reminder
            (*retQuery)[6].getUInt32(), // repeat
            (*retQuery)[7].getUInt32()  // time_zone
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffsetByField(uint32_t offset,
                                                               uint32_t limit,
                                                               EventsTableFields field,
                                                               const char *str)
{
    std::vector<EventsTableRow> ret;

    assert(0 && "Not implemented");

    return ret;
}

uint32_t EventsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM events;");

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t EventsTable::countByFieldId(const char *field, uint32_t id)
{
    assert(0 && "Not implemented");

    return 0;
}
