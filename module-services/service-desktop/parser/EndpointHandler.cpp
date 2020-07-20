#include "EndpointHandler.hpp"
#include "Contacts.hpp"
#include "Messages.hpp"
#include "ParserUtils.hpp"
#include "SMSRecord.hpp"
#include "Service/Common.hpp"
#include "Service/Service.hpp"
#include "log/log.hpp"
#include "messages/DBContactMessage.hpp"
#include "messages/DBSMSMessage.hpp"
#include "messages/DBSMSTemplateMessage.hpp"
#include "messages/QueryMessage.hpp"
#include "queries/phonebook/QueryContactGet.hpp"
#include "queries/sms/QuerySMSGetByID.hpp"
#include "queries/sms/QuerySMSGetByContactID.hpp"
#include "queries/sms/QuerySMSGetByText.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "queries/sms/QuerySMSTemplateGetByID.hpp"
#include "queries/sms/QuerySmsThreadMarkAsRead.hpp"
#include "json/json11.hpp"
#include <string>

using namespace ParserStateMachine;

std::shared_ptr<ContactHelper> EndpointHandler::contactHelper;
std::shared_ptr<MessageHelper> EndpointHandler::messageHelper;

std::string EndpointHandler::buildResponseStr(std::size_t responseSize, std::string responsePayloadString)
{
    std::string responsePayloadSizeStr = std::to_string(responseSize);
    while (responsePayloadSizeStr.length() < message::size_length) {
        responsePayloadSizeStr.insert(0, 1, '0');
    }

    std::string responseStr = message::endpointChar + responsePayloadSizeStr + responsePayloadString;
    return responseStr;
}

std::string EndpointHandler::createSimpleResponse(sys::ReturnCodes status,
                                                  int endpoint,
                                                  uint32_t uuid,
                                                  json11::Json body)
{
    json11::Json responseJson = json11::Json::object{
        {ParserStateMachine::json::endpoint, endpoint},
        {ParserStateMachine::json::status,
         static_cast<int>(status == sys::ReturnCodes::Success ? http::Code::OK : http::Code::InternalServerError)},
        {ParserStateMachine::json::uuid, std::to_string(uuid)},
        {ParserStateMachine::json::body, body}};
    return EndpointHandler::buildResponseStr(responseJson.dump().size(), responseJson.dump());
}

std::string EndpointHandler::createSimpleResponse(bool status, int endpoint, uint32_t uuid, json11::Json body)
{
    json11::Json responseJson = json11::Json::object{
        {ParserStateMachine::json::endpoint, endpoint},
        {ParserStateMachine::json::status, static_cast<int>(status ? http::Code::OK : http::Code::InternalServerError)},
        {ParserStateMachine::json::uuid, std::to_string(uuid)},
        {ParserStateMachine::json::body, body}};

    return EndpointHandler::buildResponseStr(responseJson.dump().size(), responseJson.dump());
}

void EndpointHandler::initHelpers(sys::Service *ownerService)
{
    contactHelper = std::make_shared<ContactHelper>(ownerService);
    messageHelper = std::make_shared<MessageHelper>(ownerService);
}

sys::ReturnCodes EndpointHandler::handleQueryMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    LOG_DEBUG("received data from DB");
    LOG_DEBUG("sender: %s", resp->sender.c_str());

    auto queryResponse = dynamic_cast<db::QueryResponse *>(resp);
    if (queryResponse == nullptr) {
        LOG_ERROR("Unexpected message.");
        return sys::ReturnCodes::Failure;
    }
    LOG_DEBUG("respTo: %d", static_cast<short>(queryResponse->responseTo));

    auto result = queryResponse->getResult();
    LOG_DEBUG("result: %s", result->debugInfo().c_str());

    if (dynamic_cast<db::query::ContactGetResult *>(result.get()) != nullptr) {
        auto responseStr = contactHelper->processQueryResult(result.get()).dump();
        MessageHandler::putToSendQueue(EndpointHandler::buildResponseStr(responseStr.size(), responseStr));
    }
    else if (dynamic_cast<db::query::smsthread::MarkAsReadResult *>(result.get()) != nullptr) {
        auto responseStr = messageHelper->processQueryResult(result.get()).dump();
        MessageHandler::putToSendQueue(EndpointHandler::buildResponseStr(responseStr.size(), responseStr));
    }
    else if (dynamic_cast<db::query::SMSGetByIDResult *>(result.get()) != nullptr) {
        auto responseBody = messageHelper->processQueryResult(result.get()).dump();
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), responseBody));
    }
    else if (dynamic_cast<db::query::SMSGetByContactIDResult *>(result.get()) != nullptr) {
        auto responseBody = messageHelper->processQueryResult(result.get());
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), responseBody));
    }
    else if (dynamic_cast<db::query::SMSGetByTextResult *>(result.get()) != nullptr) {
        auto responseBody = messageHelper->processQueryResult(result.get());
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), responseBody));
    }
    else if (dynamic_cast<db::query::SMSTemplateGetByIDResult *>(result.get()) != nullptr) {
        LOG_DEBUG("template test");
        auto responseBody = messageHelper->processQueryResult(result.get());
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), responseBody));
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::handleContactsMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

    if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
        resp->responseTo == MessageType::DBContactGetByID) {

        auto status = contactHelper->updateContact(dynamic_cast<DBContactResponseMessage *>(resp)->records->front());
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            status, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
    }
    else if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
             resp->responseTo == MessageType::DBContactAdd) {

        LOG_DEBUG("Contact added! ret: %d", (resp->retCode == sys::ReturnCodes::Success ? 1 : 0));
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
    }
    else if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
             resp->responseTo == MessageType::DBContactRemove) {

        LOG_DEBUG("Contact removed! ret: %d", (resp->retCode == sys::ReturnCodes::Success ? 1 : 0));
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::handleMessagesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

    if (resp->responseTo == MessageType::DBSMSGetCount) {

        auto response = dynamic_cast<DBSMSResponseMessage *>(resp);
        LOG_DEBUG("SMS count: %" PRIu32 "", response->count);
        json11::Json body = json11::Json::object{{json::messages::count, static_cast<int>(response->count)}};
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), body));
    }
    else if (resp->responseTo == MessageType::DBSMSGetSMSLimitOffset ||
             resp->responseTo == MessageType::DBSMSGetSMSLimitOffsetByThreadID) {

        auto response = dynamic_cast<DBSMSResponseMessage *>(resp);
        auto records  = std::move(response->records);

        json11::Json::array recordsArray;
        for (auto record : *records) {
            recordsArray.emplace_back(messageHelper->to_json(record));
        }

        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), recordsArray));
    }
    else if (resp->responseTo == MessageType::DBSMSRemove) {

        LOG_DEBUG("SMS removed");
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID()));
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::handleMessageTemplatesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

    if (resp->responseTo == MessageType::DBSMSTemplateGetCount) {

        auto response = dynamic_cast<DBSMSTemplateResponseMessage *>(resp);
        LOG_DEBUG("SMS Template count: %" PRIu32 "", response->count);
        json11::Json body = json11::Json::object{{json::messages::count, static_cast<int>(response->count)}};
        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), body));
    }
    else if (resp->responseTo == MessageType::DBSMSTemplateGetLimitOffset) {

        auto response = dynamic_cast<DBSMSTemplateResponseMessage *>(resp);
        auto records  = std::move(response->records);

        json11::Json::array recordsArray;
        for (auto record : *records) {
            recordsArray.emplace_back(messageHelper->to_json(record));
        }

        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), recordsArray));
    }
    else if (resp->responseTo == MessageType::DBSMSTemplateAdd ||
             resp->responseTo == MessageType::DBSMSTemplateRemove ||
             resp->responseTo == MessageType::DBSMSTemplateUpdate) {

        MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
            resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID()));
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::contacts(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    // process contact request

    switch (static_cast<http::Method>(httpMethod)) {
    case http::Method::get:
        contactHelper->requestDataFromDB(body, uuid);
        break;
    case http::Method::post: // update entry
        contactHelper->requestContactByID(body, uuid);
        break;
    case http::Method::put:
        contactHelper->createDBEntry(body, uuid);
        break;
    case http::Method::del:
        contactHelper->deleteDBEntry(body, uuid);
        break;
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::messages(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    // process messages request

    switch (static_cast<http::Method>(httpMethod)) {
    case http::Method::get:
        messageHelper->requestDataFromDB(body, uuid);
        break;
    case http::Method::post: // update entry
        messageHelper->updateDBEntry(body, uuid);
        break;
    case http::Method::put:
        messageHelper->createDBEntry(body, uuid);
        break;
    case http::Method::del:
        messageHelper->deleteDBEntry(body, uuid);
        break;
    }

    return sys::ReturnCodes::Success;
}
