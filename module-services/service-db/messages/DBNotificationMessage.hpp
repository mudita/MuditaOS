#pragma once

#include "DBMessage.hpp"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include <memory>

namespace DB
{
    enum class NotificationType
    {
        Updated,
        Added,
        Removed
    };
    enum class BaseType
    {
        AlarmDB,
        CalllogDB,
        ContacstDB,
        NotesDB,
        SettingsDB,
        SmsDB
    };
} // namespace DB

class DBNotificationMessage : public sys::DataMessage
{
  public:
    DBNotificationMessage(MessageType messageType, DB::NotificationType notificationType, DB::BaseType baseType);

    DB::NotificationType notificationType;
    DB::BaseType baseType;
};

class DBNotificationResponseMessage : public sys::ResponseMessage
{
  public:
    DBNotificationResponseMessage(uint32_t retCode);

    uint32_t retCode;
};
