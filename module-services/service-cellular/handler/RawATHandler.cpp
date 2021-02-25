#include "RawATHandler.hpp"
#include <Modem/ATCommon.hpp>
#include <module-sys/Service/Message.hpp>
#include <service-desktop/endpoints/developerMode/DeveloperModeEndpoint.hpp>
#include <service-desktop/endpoints/developerMode/event/ATRequest.hpp>

namespace cellular
{
    RawATHandler::RawATHandler(at::Channel &channel) : channel(channel)
    {}

    auto RawATHandler::handle(sdesktop::developerMode::DeveloperModeRequest *request) -> sys::MessagePointer
    {
        auto event = dynamic_cast<sdesktop::developerMode::ATResponseEvent *>(request->event.get());
        assert(event);
        auto ret = channel.cmd(event->getCommand(), event->getTimeout());
        event->setResponse(ret.response);
        return std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(
            std::make_unique<sdesktop::developerMode::ATResponseEvent>(*event));
    } // namespace cellular
} // namespace cellular
