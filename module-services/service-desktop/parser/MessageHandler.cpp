#include "MessageHandler.hpp"
#include "EndpointHandler.hpp"
#include "Service/Service.hpp"
#include "log/log.hpp"
#include "ParserUtils.hpp"

using namespace ParserStateMachine;

xQueueHandle MessageHandler::sendQueue;

MessageHandler::MessageHandler(std::string &message, sys::Service *OwnerService)
    : messageJson(json11::Json::parse(message, JsonErrorMsg)), OwnerServicePtr(OwnerService)
{}

void MessageHandler::processMessage()
{
    auto body          = messageJson[json::body];
    int endpoint       = messageJson[json::endpoint].number_value();
    unsigned long uuid = messageJson[json::uuid].number_value();
    int method         = messageJson[json::method].number_value();

    LOG_DEBUG(
        "[MsgHandler]\nmethod: %d\nendpoint: %d\nuuid: %lu\nbody: %s\n", method, endpoint, uuid, body.dump().c_str());

    auto endpointHandler = std::make_unique<EndpointHandler>();
    std::string responseStr;

    switch (static_cast<Endpoint>(endpoint)) {
    case Endpoint::deviceInfo:
        endpointHandler->deviceInfo(method, uuid, body, responseStr, OwnerServicePtr);
        LOG_DEBUG("[ENDPOINT] DeviceInfo: %s", responseStr.c_str());
        putToSendQueue(responseStr);
        break;
    case Endpoint::update:
        endpointHandler->update(method, uuid, body, responseStr, OwnerServicePtr);
        LOG_DEBUG("[ENDPOINT] Update: %s", responseStr.c_str());
        putToSendQueue(responseStr);
        break;
    case Endpoint::backup:
        endpointHandler->backup(method, uuid, body, responseStr, OwnerServicePtr);
        LOG_DEBUG("[ENDPOINT] Backup: %s", responseStr.c_str());
        putToSendQueue(responseStr);
        break;
    case Endpoint::restore:
        endpointHandler->restore(method, uuid, body, responseStr, OwnerServicePtr);
        LOG_DEBUG("[ENDPOINT] Restore: %s", responseStr.c_str());
        putToSendQueue(responseStr);
        break;
    default:
        break;
    }
}

void MessageHandler::putToSendQueue(std::string msg)
{
#if defined(TARGET_RT1051)
    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        auto *responseString = new std::string(msg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
    }
#endif
}
