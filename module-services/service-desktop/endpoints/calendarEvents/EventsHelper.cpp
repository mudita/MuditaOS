#include "EventsHelper.hpp"
#include "Common/Query.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include "EventsRecord.hpp"
#include "api/DBServiceAPI.hpp"
#include <queries/calendar/QueryEventsGetAll.hpp>
#include <queries/calendar/QueryEventsGet.hpp>
#include <queries/calendar/QueryEventsAdd.hpp>
#include <queries/calendar/QueryEventsEdit.hpp>
#include <queries/calendar/QueryEventsRemove.hpp>
#include "utf8/UTF8.hpp"
#include <memory>
#include <string>
#include <ctime>
#include <fstream>
#include <module-utils/icalendarlib/icalendar.h>

using namespace parserFSM;

auto EventsHelper::reminderToICS(uint32_t reminder) -> list<Alarm> *
{
    auto NewEvent = new Event;
    return NewEvent->Alarms;
}

auto EventsHelper::reminderFromICS(list<Alarm> *alarm) -> uint32_t
{
    return 0;
}
auto EventsHelper::repeatOptionToICS(uint32_t repeat) -> Recurrence
{
    Event *NewEvent = new Event;
    return NewEvent->RRule;
}
auto EventsHelper::repeatOptionFromICS(const Recurrence &frequency) -> uint32_t
{
    return 0;
}

auto EventsHelper::eventRecordsToICS(std::unique_ptr<std::vector<EventsRecord>> records) -> const char *
{
    const char *icsFile = "calendar.ics";

    //    SearchQuery.Criteria.From = "20100927T000000";
    //    SearchQuery.Criteria.To   = "20100927T235959";
    //    // SearchQuery.Criteria.To[HOUR] = 23;
    //    // SearchQuery.Criteria.To[MINUTE] = 59;
    //    // SearchQuery.Criteria.To[SECOND] = 59;

    ICalendar Calendar(icsFile);

    for (auto currentRecord : *records) {
        auto currentEvent     = new Event();
        currentEvent->Summary = currentRecord.title;
        currentEvent->DtStart = TimePointToString(currentRecord.date_from);
        currentEvent->DtEnd   = TimePointToString(currentRecord.date_till);
        currentEvent->RRule   = repeatOptionToICS(currentRecord.repeat);
        currentEvent->Alarms  = reminderToICS(currentRecord.reminder);
        Calendar.AddEvent(currentEvent);
    }

    return icsFile;
}

auto EventsHelper::eventRecordsFromICS(const char *icsFile) -> std::vector<EventsRecord>
{
    /// TODO: Add repeat -> freq?? and reminder -> Alarm
    /// TODO: get List of events or transfer one event?? (list will be faster and more universal)
    ICalendar Calendar(icsFile);
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(&Calendar);
    SearchQuery.ResetPosition();

    std::vector<EventsRecord> records;

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL) {
        EventsRecord currentRecord;
        currentRecord.title     = CurrentEvent->Summary;
        currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
        currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
        currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
        currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

        records.push_back(currentRecord);
    }
    return records;
}

auto EventsHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    EventsRecord record = fromICS(context.getICS()); /// TODO: Get Event from ICS BY ID

    auto query    = std::make_unique<db::query::events::Add>(record);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventResult = dynamic_cast<db::query::events::AddResult *>(result)) {

                context.setResponseStatus(EventResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
                // MessageHandler::putToSendQueue(context.createSimpleResponse());
                /// TODO: Create simple responce for ics format
                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
    return sys::ReturnCodes::Success;
}

auto EventsHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{
    //    DBServiceAPI::GetQuery(
    //            application, db::Interface::Name::Events, std::make_unique<db::query::events::Edit>(*record));

    using namespace db::query::smsthread;

    auto query = std::make_unique<db::query::smsthread::MarkAsRead>(
        context.getBody()[json::messages::threadID].int_value(),
        (context.getBody()[json::messages::isUnread].bool_value() ? MarkAsRead::Read::False : MarkAsRead::Read::True));

    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto SMSResult = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(result)) {

                context.setResponseStatus(SMSResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());

                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));

    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSThread, std::move(query));
    return sys::ReturnCodes::Success;
}

auto EventsHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    // If init GetAll()
    // If not init Get()
    auto query = std::make_unique<db::query::events::GetAll>();

    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventsResult = dynamic_cast<db::query::events::GetAllResult *>(result)) {
                context.setResponseICS(EventsHelper::eventRecordsToICS(std::move(EventsResult->getResult())));
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));

    return sys::ReturnCodes::Success;
}

auto EventsHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
{
    auto id       = context.getBody()[json::messages::id].int_value();
    auto query    = std::make_unique<db::query::SMSRemove>(id);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto SMSTemplateResult = dynamic_cast<db::query::SMSRemoveResult *>(result)) {

                context.setResponseStatus(SMSTemplateResult->getResults() ? http::Code::OK
                                                                          : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                return true;
            }
            else {
                return false;
            }
        },
        context);

    query->setQueryListener(std::move(listener));
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMS, std::move(query));
    return sys::ReturnCodes::Success;
}
