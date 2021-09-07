#include "ATRequest.hpp"
#include <service-desktop/endpoints/developerMode/DeveloperModeHelper.hpp>

namespace sdesktop::developerMode
{
    ATResponseEvent::ATResponseEvent(std::string command, std::chrono::milliseconds timeout)
        : command(std::move(command)), timeout(timeout)
    {
        context.setResponseStatus(parserFSM::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object{{parserFSM::json::developerMode::ATResponse, ""}});
    }

    void ATResponseEvent::setResponse(const std::vector<std::string> &response)
    {
        context.setResponseBody(json11::Json::object{{parserFSM::json::developerMode::ATResponse, response}});
        context.setResponseStatus(parserFSM::http::Code::OK);
    }

} // namespace sdesktop::developerMode
