#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>
#include <string>
#include <time/time_conversion.hpp>
#include "service-cellular/api/CellularServiceAPI.hpp"

std::string EndpointHandler::buildResponseStr(std::size_t responseSize, std::string responsePayloadString)
{
    std::string responsePayloadSizeStr = std::to_string(responseSize);
    while (responsePayloadSizeStr.length() < parserutils::message::size_length) {
        responsePayloadSizeStr.insert(0, 1, '0');
    }

    std::string responseStr =
        static_cast<char>(parserutils::message::Type::endpoint) + responsePayloadSizeStr + responsePayloadString;
    return responseStr;
}
