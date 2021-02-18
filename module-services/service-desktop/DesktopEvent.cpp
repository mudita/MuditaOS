#include "service-desktop/DesktopEvent.hpp"
#include <service-desktop/parser/MessageHandler.hpp>

namespace sdesktop
{
    void Event::send()
    {
        parserFSM::MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
} // namespace sdesktop
