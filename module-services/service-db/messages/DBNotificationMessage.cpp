// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBNotificationMessage.hpp"

#include "BaseInterface.hpp" // for Interface, Interface::Name
#include "Common/Query.hpp"  // for Query, Query::Type, Query::Type::Create, Query::Type::Delete, Query::Type::Update
#include "MessageType.hpp"   // for MessageType, MessageType::DBServiceNotification

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
