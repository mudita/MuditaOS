#pragma once

#include "DBMessage.hpp"
#include "Service/Message.hpp"
#include <module-db/Interface/BaseInterface.hpp>
#include <module-db/Common/Query.hpp>
#include <memory>

namespace db
{
    class EventsMessage : public sys::DataMessage
    {
      public:
        EventsMessage(db::Interface::Name interface, Query::Type type);
        db::Interface::Name interface;
        const Query::Type type;
    };
} // namespace db
