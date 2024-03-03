// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <module-db/Common/Query.hpp>
#include <module-db/Interface/BaseInterface.hpp>

namespace db
{
    class NotificationMessage : public sys::DataMessage
    {
      public:
        NotificationMessage(db::Interface::Name interface, Query::Type type, std::optional<std::uint32_t> recordId);
        const db::Interface::Name interface;
        const Query::Type type;
        const std::optional<std::uint32_t> recordId;

        bool dataModified();
    };
} // namespace db
