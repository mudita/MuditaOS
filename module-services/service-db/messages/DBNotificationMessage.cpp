// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBNotificationMessage.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>

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
