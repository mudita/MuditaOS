#include "DBNotificationMessage.hpp"

namespace db
{
    NotificationMessage::NotificationMessage(db::Interface::Name interface, Type type)
        : sys::DataMessage(MessageType::DBServiceNotification), interface(interface), type(type)
    {}
} // namespace db
