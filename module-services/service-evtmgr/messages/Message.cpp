#include "Message.hpp"
#include <Service/Service.hpp>

namespace sevm
{
    Message::Message(MessageType messageType) : DataMessage(messageType)
    {}
} // namespace sevm
