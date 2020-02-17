#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "json/json11.hpp"
#include <algorithm>

#if defined(TARGET_RT1051)
extern "C"
{
#include "../rt1051/include/virtual_com.h"
}
#include "../rt1051/Worker.hpp"
#else
#include "../linux/Worker.hpp"
#endif

extern xQueueHandle USBSendQueue;

sys::ReturnCodes EndpointHandler::battery(uint8_t method)
{

    if (method == static_cast<uint8_t>(ParserUtils::Method::Get))
    {
        //sample json response
        json11::Json responseBodyJson = json11::Json::object({{"level", 75}, {"charging", true}, {"maximumCapacity", 100}});

        int statusCode = static_cast<int>(ParserUtils::Code::OK);

        json11::Json responsePayloadJson = json11::Json::object({{"endpoint", 0}, {"status", statusCode}, {"body", responseBodyJson}});

        std::string responseSizeStr = EndpointHandler::rspPayloadSizeToStr(responsePayloadJson.dump().size());
        std::string responseHeaderStr = "#" + responseSizeStr;
        std::string responseStr = responseHeaderStr + responsePayloadJson.dump();
        LOG_DEBUG("EndpointBattery responseStr [%s]", responseStr.c_str());

        std::string *response_string = new std::string(responseStr);
        xQueueSend(USBSendQueue, &response_string, portMAX_DELAY);

        return sys::ReturnCodes::Success;
    }
    return sys::ReturnCodes::Failure;
}


std::string EndpointHandler::rspPayloadSizeToStr(std::size_t rspSize)
{
    std::string rspPayloadSizeStr = std::to_string(rspSize);
    while (rspPayloadSizeStr.length() < 9)
    {
        rspPayloadSizeStr.insert(0, 1, '0');
    }
    return rspPayloadSizeStr;
}
