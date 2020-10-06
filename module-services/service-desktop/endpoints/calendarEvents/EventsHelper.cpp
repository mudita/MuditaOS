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

void ICSegzample()
{
    Date a, b;

    a = "20080627T140000";
    b = "20080626T100000";

    a[MINUTE] -= 65;

    cout << a << endl;
    cout << a.Difference(b, HOUR) << endl;

    ICalendar Calendar("calendar.ics");
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(&Calendar);

    srand(time(NULL));

    SearchQuery.Criteria.From = "20100927T000000";
    SearchQuery.Criteria.To   = "20100927T235959";
    // SearchQuery.Criteria.To[HOUR] = 23;
    // SearchQuery.Criteria.To[MINUTE] = 59;
    // SearchQuery.Criteria.To[SECOND] = 59;

    SearchQuery.ResetPosition();

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL) {
        cout << CurrentEvent->UID << endl;
        cout << CurrentEvent->DtStart.Format() << endl;
        cout << CurrentEvent->DtEnd.Format() << endl;
        cout << CurrentEvent->Summary << endl;
        cout << CurrentEvent->Categories << endl;
        cout << "\t" << CurrentEvent->RRule.Freq << endl;
        cout << "\t" << CurrentEvent->RRule.Interval << endl;
        cout << "\t" << CurrentEvent->RRule.Until << endl << endl;

        // CurrentEvent->BaseEvent->Description = "aasfjanfkjahsf";
        // Calendar.ModifyEvent(CurrentEvent->BaseEvent);
    }

    Event *NewEvent   = new Event;
    NewEvent->Summary = "test";
    a.SetToNow();
    NewEvent->DtStart = a;
    Calendar.AddEvent(NewEvent);
}
auto EventsHelper::toICS(EventsRecord record) -> std::string
{
    Date from, till;
    // TIMEPOINT TO STRING
    from = "20080627T140000";
    till = "20080626T100000";

    ICalendar Calendar("calendar.ics");

    Event *NewEvent   = new Event;
    NewEvent->Summary = "test";
    a.SetToNow();
    NewEvent->DtStart = a;
    Calendar.AddEvent(NewEvent);
}

auto EventsHelper::fromICS(std::string msgJson) -> EventsRecord
{}

auto EventsHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    EventsRecord record = fromICS(context.getICS());

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
                context.setResponseICS(EventsHelper::toICS(std::move(EventsResult->getResult())));
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
