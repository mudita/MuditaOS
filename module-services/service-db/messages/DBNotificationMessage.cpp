#include "DBNotificationMessage.hpp"

namespace db
{
    NotificationMessage::NotificationMessage(MessageType messageType, db::Interface::Name interface)
        : sys::DataMessage(messageType), interface(interface)
    {}
} // namespace db
