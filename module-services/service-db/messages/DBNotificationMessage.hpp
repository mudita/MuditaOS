/*
 * DBNotificationMessage.hpp
 *
 *  Created on: 18 lip 2019
 *      Author: kuba
 */

#ifndef MODULE_SERVICES_SERVICE_DB_MESSAGES_DBNOTIFICATIONMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_DB_MESSAGES_DBNOTIFICATIONMESSAGE_HPP_

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
    DBNotificationMessage(MessageType messageType, DB::NotificationType notificationType, DB::BaseType baseType)
        : sys::DataMessage(messageType), notificationType(notificationType), baseType(baseType){};
    ~DBNotificationMessage() override = default;

    DB::NotificationType notificationType;
    DB::BaseType baseType;
};

class DBNotificationResponseMessage : public sys::ResponseMessage
{
  public:
    DBNotificationResponseMessage(uint32_t retCode) : sys::ResponseMessage(), retCode(retCode){};
    virtual ~DBNotificationResponseMessage(){};

    uint32_t retCode;
};

#endif /* MODULE_SERVICES_SERVICE_DB_MESSAGES_DBNOTIFICATIONMESSAGE_HPP_ */
