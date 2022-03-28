// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"

#include <Service/Message.hpp>
#include <module-db/Common/Query.hpp>
#include <module-db/Interface/BaseInterface.hpp>

#include <memory>
namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        NotificationMessage(db::Interface::Name interface, Query::Type type, uint32_t recordId);
        const db::Interface::Name interface;
        const Query::Type type;
        const uint32_t recordId;

        bool dataModified();
    };
} // namespace db
