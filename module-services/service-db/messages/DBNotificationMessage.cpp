#include "DBNotificationMessage.hpp"

namespace db
{
    NotificationMessage::NotificationMessage(db::Interface::Name interface, Query::Type type)
        : sys::DataMessage(MessageType::DBServiceNotification), interface(interface), type(type)
    {}

    bool NotificationMessage::dataModified()
    {
        return type == db::Query::Type::Create || type == db::Query::Type::Update || type == db::Query::Type::Delete;
    }
} // namespace db
