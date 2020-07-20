#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"

using namespace ParserStateMachine;

std::string EndpointHandler::buildResponseStr(std::size_t responseSize, std::string responsePayloadString)
{
    std::string responsePayloadSizeStr = std::to_string(responseSize);
    while (responsePayloadSizeStr.length() < message::size_length) {
        responsePayloadSizeStr.insert(0, 1, '0');
    }

    std::string responseStr = message::endpointChar + responsePayloadSizeStr + responsePayloadString;
    return responseStr;
}
