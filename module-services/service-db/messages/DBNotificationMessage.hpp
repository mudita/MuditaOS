// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/BaseInterface.hpp> // for Interface, Interface::Name
#include <module-db/Common/Query.hpp>            // for Query, Query::Type
#include <memory>

#include "DBMessage.hpp"
#include "Service/Message.hpp" // for DataMessage

namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        NotificationMessage(db::Interface::Name interface, Query::Type type);
        const db::Interface::Name interface;
        const Query::Type type;

        bool dataModified();
    };
} // namespace db
