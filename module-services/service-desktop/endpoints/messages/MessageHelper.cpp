#include "MessageHelper.hpp"
#include "Common/Query.hpp"
#include "ParserUtils.hpp"
#include "PhoneNumber.hpp"
#include "SMSRecord.hpp"
#include "SMSTemplateRecord.hpp"
#include "Service/Common.hpp"
#include "ThreadRecord.hpp"
#include "api/DBServiceAPI.hpp"
#include "queries/sms/QuerySMSGet.hpp"
#include "queries/sms/QuerySMSGetByContactID.hpp"
#include "queries/sms/QuerySMSGetByThreadID.hpp"
#include "queries/sms/QuerySMSGetByID.hpp"
#include "queries/sms/QuerySMSGetByText.hpp"
#include "queries/sms/QuerySMSGetCount.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "queries/sms/QuerySMSRemove.hpp"
#include "queries/sms/QuerySMSTemplateGet.hpp"
#include "queries/sms/QuerySMSTemplateRemove.hpp"
#include "queries/sms/QuerySMSTemplateUpdate.hpp"
#include "queries/sms/QuerySMSTemplateAdd.hpp"
#include "queries/sms/QuerySMSTemplateGetByID.hpp"
#include "queries/sms/QuerySMSTemplateGetCount.hpp"
#include "queries/sms/QuerySmsThreadMarkAsRead.hpp"
#include "utf8/UTF8.hpp"
#include "json/json11.hpp"
#include <memory>
#include <string>

using namespace parserFSM;

auto MessageHelper::to_json(SMSRecord record) -> json11::Json
{

    auto recordEntry = json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                                            {json::messages::date, static_cast<int>(record.date)},
                                            {json::messages::dateSent, static_cast<int>(record.dateSent)},
                                            {json::messages::id, static_cast<int>(record.ID)},
                                            {json::messages::messageBody, record.body.c_str()},
                                            {json::messages::phoneNumber, record.number.getEntered()},
                                            {json::messages::type, static_cast<int>(record.type)},
                                            {json::messages::threadID, static_cast<int>(record.threadID)}};
    return recordEntry;
}

auto MessageHelper::to_json(SMSTemplateRecord record) -> json11::Json
{

    auto recordEntry = json11::Json::object{{json::messages::id, static_cast<int>(record.ID)},
                                            {json::messages::templateText, record.text.c_str()}};
    return recordEntry;
}

auto MessageHelper::to_json(ThreadRecord record) -> json11::Json
{

    auto recordEntry =
        json11::Json::object{{json::messages::contactID, static_cast<int>(record.contactID)},
                             {json::messages::date, static_cast<int>(record.date)},
                             {json::messages::thread::msgCount, static_cast<int>(record.msgCount)},
                             {json::messages::id, static_cast<int>(record.ID)},
                             {json::messages::thread::snippet, record.snippet.c_str()},
                             {json::messages::isUnread, record.isUnread()},
                             {json::messages::type, static_cast<int>(record.type)},
                             {json::messages::thread::unreadMsgCount, static_cast<int>(record.unreadMsgCount)}};
    return recordEntry;
}

auto MessageHelper::from_json(json11::Json msgJson) -> SMSTemplateRecord
{
    SMSTemplateRecord record;

    record.text = UTF8(msgJson[json::messages::templateText].string_value());

    return record;
}

auto MessageHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{

    if (context.getBody()[json::messages::msgTemplate].bool_value() == true) {
        return requestTemplate(context);
    }
    else {
        return requestSMS(context);
    }
}

auto MessageHelper::requestSMS(Context &context) -> sys::ReturnCodes
{
    if (context.getBody()[json::messages::count].bool_value() == true) // get messages count
    {
        auto query = std::make_unique<db::query::SMSGetCount>();

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetCountResult *>(result)) {
                    auto id = SMSResult->getResults();

                    context.setResponseBody(json11::Json::object{{json::messages::count, static_cast<int>(id)}});
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
    }
    else if (context.getBody()[json::messages::id].int_value() != 0) { // messages search

        auto query = std::make_unique<db::query::SMSGetByID>(context.getBody()[json::messages::id].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetByIDResult *>(result)) {

                    context.setResponseBody(MessageHelper::to_json(SMSResult->getResults()));
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
    }
    else if (context.getBody()[json::messages::contactID].int_value() != 0) {

        auto query =
            std::make_unique<db::query::SMSGetByContactID>(context.getBody()[json::messages::contactID].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetByContactIDResult *>(result)) {

                    json11::Json::array SMSarray;
                    for (auto record : SMSResult->getResults()) {
                        SMSarray.emplace_back(MessageHelper::to_json(record));
                    }

                    context.setResponseBody(SMSarray);
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
    }
    else if (context.getBody()[json::messages::threadID].int_value() != 0) {

        auto query =
            std::make_unique<db::query::SMSGetByThreadID>(context.getBody()[json::messages::threadID].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetByThreadIDResult *>(result)) {

                    json11::Json::array SMSarray;
                    for (auto record : SMSResult->getResults()) {
                        SMSarray.emplace_back(MessageHelper::to_json(record));
                    }

                    context.setResponseBody(SMSarray);
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
    }
    else if (context.getBody()[json::messages::messageBody].string_value().empty() == false) {
        auto query =
            std::make_unique<db::query::SMSGetByText>(context.getBody()[json::messages::messageBody].string_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetByTextResult *>(result)) {

                    json11::Json::array SMSarray;
                    for (auto record : SMSResult->getResults()) {
                        SMSarray.emplace_back(MessageHelper::to_json(record));
                    }

                    context.setResponseBody(SMSarray);
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
    }
    else // get messages
    {
        auto query = std::make_unique<db::query::SMSGet>(context.getBody()[json::messages::count].int_value(),
                                                         context.getBody()[json::messages::offset].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetResult *>(result)) {

                    json11::Json::array SMSarray;
                    for (auto record : SMSResult->getRecords()) {
                        SMSarray.emplace_back(MessageHelper::to_json(record));
                    }

                    context.setResponseBody(SMSarray);
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
    }

    return sys::ReturnCodes::Success;
}

auto MessageHelper::requestTemplate(Context &context) -> sys::ReturnCodes
{
    if (context.getBody()[json::messages::count].bool_value() == true) // get templates count
    {
        auto query = std::make_unique<db::query::SMSTemplateGetCount>();

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSTemplateGetCountResult *>(result)) {
                    auto id = SMSResult->getResults();

                    context.setResponseBody(json11::Json::object{{json::messages::count, static_cast<int>(id)}});
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }
    else if (context.getBody()[json::messages::id].int_value() != 0) { // templates search
        auto query = std::make_unique<db::query::SMSTemplateGetByID>(context.getBody()[json::messages::id].int_value());

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateGetByIDResult *>(result)) {

                    context.setResponseBody(MessageHelper::to_json(SMSTemplateResult->getResults()));
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }
    else // get messages templates
    {
        auto query = std::make_unique<db::query::SMSTemplateGet>(context.getBody()[json::messages::offset].int_value(),
                                                                 context.getBody()[json::messages::count].int_value());
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateGetResult *>(result)) {

                    json11::Json::array SMSTemplateArray;
                    for (auto record : SMSTemplateResult->getResults()) {
                        SMSTemplateArray.emplace_back(to_json(record));
                    }

                    context.setResponseBody(SMSTemplateArray);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    }

    return sys::ReturnCodes::Success;
}

auto MessageHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{

    if (context.getBody()[json::messages::msgTemplate].bool_value() == true) {
        return createTemplate(context);
    }
    else {
        return createSMS(context);
    }
}

auto MessageHelper::createTemplate(Context &context) -> sys::ReturnCodes
{
    SMSTemplateRecord record = from_json(context.getBody());

    auto query    = std::make_unique<db::query::SMSTemplateAdd>(record);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateAddResult *>(result)) {

                context.setResponseStatus(SMSTemplateResult->getResult() ? http::Code::OK
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
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));

    return sys::ReturnCodes::Success;
}

auto MessageHelper::createSMS(Context &context) -> sys::ReturnCodes
{
    return sys::ReturnCodes::Success;
}

auto MessageHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
{

    if (context.getBody()[json::messages::msgTemplate].bool_value() == true) {
        return deleteTemplate(context);
    }
    else {
        return deleteSMS(context);
    }
}

auto MessageHelper::deleteSMS(Context &context) -> sys::ReturnCodes
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

auto MessageHelper::deleteTemplate(Context &context) -> sys::ReturnCodes
{
    auto id       = context.getBody()[json::messages::id].int_value();
    auto query    = std::make_unique<db::query::SMSTemplateRemove>(id);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateRemoveResult *>(result)) {

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
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));
    return sys::ReturnCodes::Success;
}

auto MessageHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{

    if (context.getBody()[json::messages::msgTemplate].bool_value() == true) {
        return updateTemplate(context);
    }
    else {
        return updateSMS(context);
    }
}

auto MessageHelper::updateSMS(Context &context) -> sys::ReturnCodes
{
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

auto MessageHelper::updateTemplate(Context &context) -> sys::ReturnCodes
{
    SMSTemplateRecord record;
    record.ID   = context.getBody()[json::messages::id].int_value();
    record.text = context.getBody()[json::messages::templateText].string_value();

    auto query    = std::make_unique<db::query::SMSTemplateUpdate>(record);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto SMSTemplateResult = dynamic_cast<db::query::SMSTemplateUpdateResult *>(result)) {

                context.setResponseStatus(SMSTemplateResult->getResult() ? http::Code::OK
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
    DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::SMSTemplate, std::move(query));

    return sys::ReturnCodes::Success;
}
