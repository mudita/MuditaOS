#include "EventsTable.hpp"
#include "module-db/Interface/EventsRecord.hpp"
//#include <module-apps/application-calendar/widgets/CalendarStyle.hpp>
//#include <module-apps/application-calendar/data/OptionParser.hpp>

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
    // Prevent duplicates using ANDs:
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) "
                       "SELECT '%q', '%q','%q', %lu, %lu "
                       "WHERE NOT EXISTS "
                       "(SELECT 1 FROM events e "
                       "WHERE e.title='%q' "
                       "AND e.date_from='%q' "
                       "AND e.date_till='%q' "
                       "AND e.reminder=%lu "
                       "AND e.repeat=%lu );",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addDaily(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u);",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{1}).c_str(),
                       TimePointToString(entry.date_till + date::days{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{2}).c_str(),
                       TimePointToString(entry.date_till + date::days{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{3}).c_str(),
                       TimePointToString(entry.date_till + date::days{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{4}).c_str(),
                       TimePointToString(entry.date_till + date::days{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{5}).c_str(),
                       TimePointToString(entry.date_till + date::days{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{6}).c_str(),
                       TimePointToString(entry.date_till + date::days{6}).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addWeekly(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u);",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{7}).c_str(),
                       TimePointToString(entry.date_till + date::days{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{21}).c_str(),
                       TimePointToString(entry.date_till + date::days{21}).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addTwoWeeks(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u);",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{28}).c_str(),
                       TimePointToString(entry.date_till + date::days{28}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{42}).c_str(),
                       TimePointToString(entry.date_till + date::days{42}).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addMonth(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u);",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{1}).c_str(),
                       TimePointToString(entry.date_till, date::months{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{2}).c_str(),
                       TimePointToString(entry.date_till, date::months{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{3}).c_str(),
                       TimePointToString(entry.date_till, date::months{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{4}).c_str(),
                       TimePointToString(entry.date_till, date::months{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{5}).c_str(),
                       TimePointToString(entry.date_till, date::months{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{6}).c_str(),
                       TimePointToString(entry.date_till, date::months{6}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{7}).c_str(),
                       TimePointToString(entry.date_till, date::months{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{8}).c_str(),
                       TimePointToString(entry.date_till, date::months{8}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{9}).c_str(),
                       TimePointToString(entry.date_till, date::months{9}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{10}).c_str(),
                       TimePointToString(entry.date_till, date::months{10}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{11}).c_str(),
                       TimePointToString(entry.date_till, date::months{11}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{12}).c_str(),
                       TimePointToString(entry.date_till, date::months{12}).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addYear(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u),"
                       "('%q', '%q','%q', %u, %u);",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{1}).c_str(),
                       TimePointToString(entry.date_till + date::years{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{2}).c_str(),
                       TimePointToString(entry.date_till + date::years{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{3}).c_str(),
                       TimePointToString(entry.date_till + date::years{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{4}).c_str(),
                       TimePointToString(entry.date_till + date::years{4}).c_str(),
                       entry.reminder,
                       entry.repeat);
}

std::vector<bool> parseOptions(const uint32_t &dataDB)
{
    const uint32_t startBit        = 16;
    const uint32_t numberOfOptions = 7;
    std::vector<bool> weekDayOptions;
    for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
        weekDayOptions.push_back(false);
    }
    for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
        if (dataDB & (1 << i)) {
            LOG_DEBUG("SET OPTION ARRAY! %d", static_cast<int>(i));
            weekDayOptions[i - startBit] = true;
        }
    }
    return weekDayOptions;
}

std::string concatenate(const char *format, ...)
{
    std::string result = format;
    return result;
}

bool EventsTable::addCustom(EventsTableRow entry)
{
    // OptionParser parser;
    std::vector<bool> weekDayOptions;
    weekDayOptions = parseOptions(entry.repeat);

    std::string request = "INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES ";
    //    for (uint32_t i = 0; i < 7 ;i++)
    //    {
    //        if (weekDayOptions[i]){
    request = request + concatenate("('%q', datetime('%q',('+%u year')),datetime('%q',('+%u year')), %u, %u), ",
                                    entry.title.c_str(),
                                    TimePointToString(entry.date_from).c_str(),
                                    1,
                                    TimePointToString(entry.date_till).c_str(),
                                    1,
                                    entry.reminder,
                                    entry.repeat);

    request = request + concatenate("('%q', datetime('%q',('+%u day')),datetime('%q',('+%u day')), %u, %u); ",
                                    entry.title.c_str(),
                                    TimePointToString(entry.date_from).c_str(),
                                    2,
                                    TimePointToString(entry.date_till).c_str(),
                                    2,
                                    entry.reminder,
                                    entry.repeat);
    //        }
    //    }
    return db->execute(request.c_str());
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
    return db->execute("UPDATE events SET title= '%q', date_from = '%q', date_till = '%q', reminder "
                       "= %u, repeat = %u WHERE _id = %u;",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
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
        (*retQuery)[0].getUInt32(),                              // ID
        (*retQuery)[1].getString(),                              // title
        TimePointFromString((*retQuery)[2].getString().c_str()), // date_from
        TimePointFromString((*retQuery)[3].getString().c_str()), // date_till
        (*retQuery)[4].getUInt32(),                              // reminder
        (*retQuery)[5].getUInt32()                               // repeat

    };
}

std::vector<EventsTableRow> EventsTable::selectByDatePeriod(TimePoint date_filter, TimePoint filter_till)
{
    auto retQuery =
        db->query("SELECT * FROM events WHERE date_from >= date('%q') AND date_till < date('%q', 'start of day');",
                  TimePointToString(date_filter).c_str(),
                  TimePointToString(filter_till).c_str());

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // title
            TimePointFromString((*retQuery)[2].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_till
            (*retQuery)[4].getUInt32(),                              // reminder
            (*retQuery)[5].getUInt32()                               // repeat
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
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // title
            TimePointFromString((*retQuery)[2].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_till
            (*retQuery)[4].getUInt32(),                              // reminder
            (*retQuery)[5].getUInt32()                               // repeat

        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffsetByDate(uint32_t offset, uint32_t limit)
{

    auto retQuery = db->query("SELECT * from events ORDER BY datetime(date_from) LIMIT %u OFFSET %u;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // title
            TimePointFromString((*retQuery)[2].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_till
            (*retQuery)[4].getUInt32(),                              // reminder
            (*retQuery)[5].getUInt32()                               // repeat
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
