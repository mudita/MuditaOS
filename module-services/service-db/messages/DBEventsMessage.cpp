#include "DBEventsMessage.hpp"

namespace db
{
    EventsMessage::EventsMessage(db::Interface::Name interface, Query::Type type)
        : sys::DataMessage(MessageType::DBServiceEvents), interface(interface), type(type)
    {}
} // namespace db
