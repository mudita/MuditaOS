#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "SMSRecord.hpp"
#include "Service/Common.hpp"
#include "Service/Service.hpp"
#include "log/log.hpp"
#include "messages/DBContactMessage.hpp"
#include "messages/DBSMSMessage.hpp"
#include "messages/DBSMSTemplateMessage.hpp"
#include "messages/MessageHelper.hpp"
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

// sys::ReturnCodes EndpointHandler::handleContactsMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
// {
//     LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

//     if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
//         resp->responseTo == MessageType::DBContactGetByID) {

//         auto status = contactHelper->updateContact(dynamic_cast<DBContactResponseMessage *>(resp)->records->front());
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             status, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
//     }
//     else if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
//              resp->responseTo == MessageType::DBContactAdd) {

//         LOG_DEBUG("Contact added! ret: %d", (resp->retCode == sys::ReturnCodes::Success ? 1 : 0));
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
//     }
//     else if (dynamic_cast<DBContactResponseMessage *>(resp) != nullptr &&
//              resp->responseTo == MessageType::DBContactRemove) {

//         LOG_DEBUG("Contact removed! ret: %d", (resp->retCode == sys::ReturnCodes::Success ? 1 : 0));
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::contacts), contactHelper->getLastUUID()));
//     }

//     return sys::ReturnCodes::Success;
// }

// sys::ReturnCodes EndpointHandler::handleMessagesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
// {
//     LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

//     if (resp->responseTo == MessageType::DBSMSGetCount) {

//         auto response = dynamic_cast<DBSMSResponseMessage *>(resp);
//         LOG_DEBUG("SMS count: %" PRIu32 "", response->count);
//         json11::Json body = json11::Json::object{{json::messages::count, static_cast<int>(response->count)}};
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), body));
//     }
//     else if (resp->responseTo == MessageType::DBSMSGetSMSLimitOffset ||
//              resp->responseTo == MessageType::DBSMSGetSMSLimitOffsetByThreadID) {

//         auto response = dynamic_cast<DBSMSResponseMessage *>(resp);
//         auto records  = std::move(response->records);

//         json11::Json::array recordsArray;
//         for (auto record : *records) {
//             recordsArray.emplace_back(messageHelper->to_json(record));
//         }

//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), recordsArray));
//     }
//     else if (resp->responseTo == MessageType::DBSMSRemove) {

//         LOG_DEBUG("SMS removed");
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID()));
//     }

//     return sys::ReturnCodes::Success;
// }

// sys::ReturnCodes EndpointHandler::handleMessageTemplatesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp)
// {
//     LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));

//     if (resp->responseTo == MessageType::DBSMSTemplateGetCount) {

//         auto response = dynamic_cast<DBSMSTemplateResponseMessage *>(resp);
//         LOG_DEBUG("SMS Template count: %" PRIu32 "", response->count);
//         json11::Json body = json11::Json::object{{json::messages::count, static_cast<int>(response->count)}};
//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), body));
//     }
//     else if (resp->responseTo == MessageType::DBSMSTemplateGetLimitOffset) {

//         auto response = dynamic_cast<DBSMSTemplateResponseMessage *>(resp);
//         auto records  = std::move(response->records);

//         json11::Json::array recordsArray;
//         for (auto record : *records) {
//             recordsArray.emplace_back(messageHelper->to_json(record));
//         }

//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID(), recordsArray));
//     }
//     else if (resp->responseTo == MessageType::DBSMSTemplateAdd ||
//              resp->responseTo == MessageType::DBSMSTemplateRemove ||
//              resp->responseTo == MessageType::DBSMSTemplateUpdate) {

//         MessageHandler::putToSendQueue(EndpointHandler::createSimpleResponse(
//             resp->retCode, static_cast<int>(Endpoint::messages), messageHelper->getLastUUID()));
//     }

//     return sys::ReturnCodes::Success;
// }
