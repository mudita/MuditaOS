#pragma once

#include <module-sys/Service/MessageForward.hpp>

namespace at
{
    class Channel;
}

namespace sdesktop::developerMode
{
    class DeveloperModeRequest;
}

namespace cellular
{

    /// This class handles Raw AT command request and returns message to be sent to requester
    class RawATHandler
    {
        at::Channel &channel;

      public:
        explicit RawATHandler(at::Channel &);
        auto handle(sdesktop::developerMode::DeveloperModeRequest *request) -> sys::MessagePointer;
    };

} // namespace cellular
