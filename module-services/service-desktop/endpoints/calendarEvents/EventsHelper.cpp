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

auto EventsHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    auto query = std::make_unique<db::query::events::GetAll>();

    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventsResult = dynamic_cast<db::query::events::GetAllResult *>(result)) {
                /// This method updates also ics file
                parser->eventRecordsToICS(EventsResult->getResult());
                context.setResponseBody(parser->stringFromICS()); /// TODO: check if it work: conv string to json
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

auto EventsHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    parser->updateICS(context.getBody());
    auto records = parser->eventRecordsFromICS();
    assert(records.size() == 1);
    EventsRecord record = records[0];

    auto query    = std::make_unique<db::query::events::Add>(record);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventResult = dynamic_cast<db::query::events::AddResult *>(result)) {
                context.setResponseStatus(EventResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
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

auto EventsHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{
    parser->updateICS(context.getBody());
    auto records = parser->eventRecordsFromICS();
    assert(records.size() == 1);
    EventsRecord record = records[0];

    auto query = std::make_unique<db::query::events::Edit>(record);

    auto listener = std::make_unique<db::EndpointListener>(
        [](db::QueryResult *result, Context context) {
            if (auto EventResult = dynamic_cast<db::query::events::EditResult *>(result)) {

                context.setResponseStatus(EventResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
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
    auto UID      = context.getBody();
    auto query    = std::make_unique<db::query::events::Remove>(5); /// TODO:change to UID
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventResult = dynamic_cast<db::query::events::RemoveResult *>(result)) {

                context.setResponseStatus(EventResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
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
