#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "json/json11.hpp"
#include <string>
#if defined(TARGET_RT1051)
extern "C"
{
#include "../rt1051/include/virtual_com.h"
}
#include "../rt1051/Worker.hpp"
#else
#include "../linux/Worker.hpp"
#endif

sys::ReturnCodes EndpointHandler::battery(uint8_t httpMethod, std::string &responseStr)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get))
    {
        //sample json response
        json11::Json responseBodyJson = json11::Json::object({{"level", 75}, {"charging", true}, {"maximumCapacity", 100}});

        int statusCode = static_cast<int>(parserutils::http::Code::OK);
        int endpoint = static_cast<int>(parserutils::Endpoint::battery);

        json11::Json responsePayloadJson = json11::Json::object(
            {{parserutils::json::endpoint, endpoint}, {parserutils::json::status, statusCode}, {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else
    {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}

std::string EndpointHandler::buildResponseStr(std::size_t responseSize, std::string responsePayloadString)
{
    std::string responsePayloadSizeStr = std::to_string(responseSize);
    while (responsePayloadSizeStr.length() < parserutils::message::size_length)
    {
        responsePayloadSizeStr.insert(0, 1, '0');
    }

    std::string responseStr = static_cast<char>(parserutils::message::type::endpoint) + responsePayloadSizeStr + responsePayloadString;
    return responseStr;
}
