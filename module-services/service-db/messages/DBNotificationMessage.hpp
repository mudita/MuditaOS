#pragma once

#include "DBMessage.hpp"
#include "Service/Message.hpp"
#include <memory>
#include <module-db/Interface/BaseInterface.hpp>

namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        /// made it CRUD
        enum class Type
        {
            Create,
            Read,
            Update,
            Delete
        };

        NotificationMessage(db::Interface::Name interface, Type type);
        db::Interface::Name interface;
        const Type type;
    };
} // namespace db
