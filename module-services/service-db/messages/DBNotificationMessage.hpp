#pragma once

#include "DBMessage.hpp"
#include "Service/Message.hpp"
#include <memory>
#include <module-db/Interface/BaseInterface.hpp>
#include <module-db/Common/Query.hpp>

namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        NotificationMessage(db::Interface::Name interface, Query::Type type);
        db::Interface::Name interface;
        const Query::Type type;
    };
} // namespace db
