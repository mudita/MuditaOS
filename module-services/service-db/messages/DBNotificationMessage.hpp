#pragma once

#include "DBMessage.hpp"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include <memory>
#include <module-db/Interface/BaseInterface.hpp>

namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        NotificationMessage(MessageType messageType, db::Interface::Name interface);
        db::Interface::Name interface;
    };
} // namespace db
