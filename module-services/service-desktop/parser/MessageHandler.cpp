#include "MessageHandler.hpp"
#include "Context.hpp"
#include "EndpointFactory.hpp"
#include "log/log.hpp"
#include "ParserUtils.hpp"
#include <memory>

using namespace ParserStateMachine;

xQueueHandle MessageHandler::sendQueue;

MessageHandler::MessageHandler(std::string &message, sys::Service *OwnerService) : OwnerServicePtr(OwnerService)
{
    try {
        messageJson = json11::Json::parse(message, JsonErrorMsg);
    }
    catch (const std::exception &e) {
        LOG_ERROR("Cannot create MessageHandler! err:%s", e.what());
    }
}

void MessageHandler::processMessage()
{
    Context context(messageJson);

    LOG_DEBUG("[MsgHandler]\nmethod: %d\nendpoint: %d\nuuid: %" PRIu32 "\nbody: %s\n",
              static_cast<int>(context.getMethod()),
              static_cast<int>(context.getEndpoint()),
              context.getUuid(),
              context.getBody().dump().c_str());

    auto handler = EndpointFactory::create(context, OwnerServicePtr);

    if (handler != nullptr) {
        handler->handle(context);
    }
    else {
        LOG_ERROR("No way to handle!");
    }
}

void MessageHandler::putToSendQueue(const std::string msg)
{
#if defined(TARGET_RT1051)
    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        auto *responseString = new std::string(msg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
    }
#endif
}
