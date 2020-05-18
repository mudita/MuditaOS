#include "DBNotificationMessage.hpp"

DBNotificationMessage::DBNotificationMessage(MessageType messageType,
                                             DB::NotificationType notificationType,
                                             DB::BaseType baseType)
    : sys::DataMessage(messageType), notificationType(notificationType), baseType(baseType)
{}

DBNotificationResponseMessage::DBNotificationResponseMessage(uint32_t retCode)
    : sys::ResponseMessage(), retCode(retCode)
{}
