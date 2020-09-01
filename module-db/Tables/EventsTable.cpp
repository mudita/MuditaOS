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
    if (!db->execute(createTableQuery.c_str())) {
        return false;
    }

    return true;
}

bool EventsTable::add(EventsTableRow entry)
{
    // Prevent duplicates using ANDs:
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) "
                       "SELECT '%q', '%q','%q', %lu, %lu, '%q' "
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
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addDaily(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q');",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{1}).c_str(),
                       TimePointToString(entry.date_till + date::days{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{2}).c_str(),
                       TimePointToString(entry.date_till + date::days{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{3}).c_str(),
                       TimePointToString(entry.date_till + date::days{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{4}).c_str(),
                       TimePointToString(entry.date_till + date::days{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{5}).c_str(),
                       TimePointToString(entry.date_till + date::days{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{6}).c_str(),
                       TimePointToString(entry.date_till + date::days{6}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str());
}

bool EventsTable::addWeekly(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q');",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{7}).c_str(),
                       TimePointToString(entry.date_till + date::days{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{21}).c_str(),
                       TimePointToString(entry.date_till + date::days{21}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str());
}

bool EventsTable::addTwoWeeks(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q');",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{28}).c_str(),
                       TimePointToString(entry.date_till + date::days{28}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{42}).c_str(),
                       TimePointToString(entry.date_till + date::days{42}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str());
}

bool EventsTable::addMonth(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q');",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{1}).c_str(),
                       TimePointToString(entry.date_till, date::months{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{2}).c_str(),
                       TimePointToString(entry.date_till, date::months{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{3}).c_str(),
                       TimePointToString(entry.date_till, date::months{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{4}).c_str(),
                       TimePointToString(entry.date_till, date::months{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{5}).c_str(),
                       TimePointToString(entry.date_till, date::months{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{6}).c_str(),
                       TimePointToString(entry.date_till, date::months{6}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{7}).c_str(),
                       TimePointToString(entry.date_till, date::months{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{8}).c_str(),
                       TimePointToString(entry.date_till, date::months{8}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{9}).c_str(),
                       TimePointToString(entry.date_till, date::months{9}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{10}).c_str(),
                       TimePointToString(entry.date_till, date::months{10}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{11}).c_str(),
                       TimePointToString(entry.date_till, date::months{11}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{12}).c_str(),
                       TimePointToString(entry.date_till, date::months{12}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str());
}

bool EventsTable::addYear(EventsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO events "
                       "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q'),"
                       "('%q', '%q', '%q', %u, %u, '%q');",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{1}).c_str(),
                       TimePointToString(entry.date_till + date::years{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{2}).c_str(),
                       TimePointToString(entry.date_till + date::years{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{3}).c_str(),
                       TimePointToString(entry.date_till + date::years{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::years{4}).c_str(),
                       TimePointToString(entry.date_till + date::years{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str());
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
            LOG_DEBUG("Set option array %d", static_cast<int>(i));
            weekDayOptions[i - startBit] = true;
        }
    }
    return weekDayOptions;
}

bool EventsTable::addCustom(EventsTableRow entry)
{
    // OptionParser parser;
    const uint32_t numberOfWeeks    = 4;
    const uint32_t numberOfWeekDays = 7;
    bool result                     = true;
    std::vector<bool> weekDayOptions;
    weekDayOptions          = parseOptions(entry.repeat);
    uint32_t incrementation = 0;

    result = result && db->execute("INSERT or IGNORE INTO events (title, date_from, date_till, reminder, repeat) VALUES"
                                   "('%q', '%q','%q', %u, %u);",
                                   entry.title.c_str(),
                                   TimePointToString(entry.date_from).c_str(),
                                   TimePointToString(entry.date_till).c_str(),
                                   entry.reminder,
                                   entry.repeat);

    auto dateFrom = getFirstWeekDay(entry.date_from);
    auto dateTill = getFirstWeekDay(entry.date_till);

    for (uint32_t i = 1; i <= numberOfWeeks; i++) {
        for (auto option : weekDayOptions) {
            if (option) {
                result =
                    result &&
                    db->execute("INSERT or IGNORE INTO events "
                                "(title, date_from, date_till, reminder, repeat, reminder_fired) VALUES"
                                "('%q', '%q','%q', %u, %u);",
                                entry.title.c_str(),
                                TimePointToString(dateFrom + date::days{incrementation}).c_str(),
                                TimePointToString(dateTill + date::days{incrementation}).c_str(),
                                entry.reminder,
                                entry.repeat,
                                TimePointToString(entry.reminder_fired).c_str());
            }
            ++incrementation;
        }
        incrementation = i * numberOfWeekDays;
    }
    return result;
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
                       "= %u, repeat = %u, reminder_fired = '%q' WHERE _id = %u;",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
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
        (*retQuery)[5].getUInt32(),                              // repeat
        TimePointFromString((*retQuery)[6].getString().c_str())  // date_till

    };
}

std::vector<EventsTableRow> EventsTable::selectByDatePeriod(TimePoint date_filter, TimePoint filter_till)
{
    auto retQuery =
        db->query("SELECT * FROM events WHERE date_from >= date('%q') AND date_from < date('%q', 'start of day');",
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
            (*retQuery)[5].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[6].getString().c_str())  // date_till
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
            (*retQuery)[5].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[6].getString().c_str())  // date_till
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
            (*retQuery)[5].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[6].getString().c_str())  // date_till
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

// mlucki
#if 0
EventsTypedTableRow EventsTable::getClosestInsideDay1(uint32_t start_date, uint32_t day_date)
{
    /*
    select 'event' as type, date_from as my_date,* from (select * from events where date_from >= 141020142 and
    event_fired = -1 ORDER BY date_from LIMIT 1) UNION ALL select 'reminder' as type, reminder as my_date,* from (select
    * from events where reminder >= 141020142 and reminder_fired = -1 ORDER BY reminder LIMIT 1) order by my_date  LIMIT
    1
     */
    auto retQuery1 = db->query("SELECT 2 AS type, date_from AS date_selected,* FROM "
                              "(SELECT * from events WHERE date_from >= %u AND date_from < %u AND event_fired = -1 "
                              "ORDER BY date_from LIMIT 1) "
                              "UNION ALL "
                              "SELECT 1 as type, reminder AS date_selected,* FROM "
                              "(SELECT * from events WHERE reminder >= %u AND reminder < %u AND reminder_fired = -1 "
                              "ORDER BY reminder LIMIT 1) "
                              "ORDER BY found_date LIMIT 1",
                              start_date,
                              day_date,
                              start_date,
                              day_date);

    //select * from events where (date_from - reminder) >= 141020142 and event_fired = -1 ORDER BY (date_from - reminder)  LIMIT 1
    //select datetime(date_from, '+' || reminder || ' minutes') AS calc_dt, * from events where calc_dt >= "2020-08-17 17:20" and event_fired = -1 and reminder <> -1 ORDER BY (calc_dt) LIMIT 1
    //select datetime(date_from, '+' || reminder || ' minutes') AS calc_dt, * from events where calc_dt >= "2020-08-17 17:20" and calc_dt <= datetime("2020-08-17", "+1 day", "-1 second") and event_fired = -1 and reminder <> -1 ORDER BY (calc_dt) LIMIT 1

    auto retQuery= db->query("SELECT DATETIME(date_from, '+' || reminder || ' minutes') AS calc_dt, * "
                               "FROM events "
                               "WHERE calc_dt >= %u "
                               "AND calc_dt <= DATETIME(%u, '+1 day', '-1 second') "
                               "AND event_fired = -1 "
                               "AND reminder <> -1 "
                               "ORDER BY calc_dt "
                               "LIMIT 1",
                               start_date,
                               day_date);



    // mlucki
    // I tu zaczynają się małe schody:
    //  - może być tak, że event jest ustawiony na jutro ale reminder (jako liczba minut przed eventem) trafia w dziś
    //      trzeba to opanować w zapytaniu
    //  - albo pytać bez filtra na "day_date" a tu w kodzie decydować czy zwrócona data jest z dzisiaj!!!

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return EventsTypedTableRow();
    }

    EventsTypedTableRow ret;

    return EventsTypedTableRow{
        /*(*retQuery)[2].getUInt32(), // ID
        (*retQuery)[3].getString(), // title
        (*retQuery)[4].getString(), // description
        (*retQuery)[5].getUInt32(), // date_from
        (*retQuery)[6].getUInt32(), // date_till
        (*retQuery)[7].getUInt32(), // reminder
        (*retQuery)[8].getUInt32(), // repeat
        (*retQuery)[9].getUInt32(), // time_zone
        (*retQuery)[1].getUInt32(), // date_selected
        EventsTypedTableRow::typeFromUint((*retQuery)[0].getUInt32()),*/
    };
}
#endif

std::vector<EventsTableRow> EventsTable::SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till)
{
    // select * from events where (date_from - reminder) >= 141020142 and event_fired = -1 ORDER BY (date_from -
    // reminder)  LIMIT 1 select datetime(date_from, '+' || reminder || ' minutes') AS calc_dt, * from events where
    // calc_dt >= "2020-08-17 17:20" and event_fired = -1 and reminder <> -1 ORDER BY (calc_dt) LIMIT 1 select
    // datetime(date_from, '+' || reminder || ' minutes') AS calc_dt, * from events where calc_dt >= "2020-08-17 17:20"
    // and calc_dt <= datetime("2020-08-17", "+1 day", "-1 second") and event_fired = -1 and reminder <> -1 ORDER BY
    // (calc_dt) LIMIT 1

    [[maybe_unused]] std::string fff = TimePointToString(TIME_POINT_INVALID).c_str();

    auto retQuery = db->query("SELECT DATETIME(date_from, '-' || reminder || ' minutes') AS calc_dt, * "
                              //"SELECT date_from AS calc_dt, * "
                              "FROM events "
                              "WHERE calc_dt >= '%q' "
                              // mlucki
                              "AND calc_dt <= DATETIME('%q', '+1 day', '-1 second') "
                              "AND reminder_fired = '%q' "
                              //  "AND reminder_fired = '1970-01-01 00:00:00' "
                              "AND reminder <> -1 "
                              "ORDER BY calc_dt ",
                              //"LIMIT 1",
                              TimePointToString(filter_from).c_str(),
                              TimePointToString(filter_till).c_str(),
                              TimePointToString(TIME_POINT_INVALID).c_str());

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[1].getUInt32(),                              // ID
            (*retQuery)[2].getString(),                              // title
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
            (*retQuery)[5].getUInt32(),                              // reminder
            (*retQuery)[6].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[7].getString().c_str())  // date_till
        });
    } while (retQuery->nextRow());

    return ret;
}
