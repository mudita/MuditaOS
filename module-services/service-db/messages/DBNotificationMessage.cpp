#include "DBNotificationMessage.hpp"

namespace db
{
    NotificationMessage::NotificationMessage(db::Interface::Name interface, Query::Type type)
        : sys::DataMessage(MessageType::DBServiceNotification), interface(interface), type(type)
    {}
} // namespace db
