// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json11.hpp>
#include "Service/Service.hpp"
#include <service-desktop/Outbox.hpp>
#include "endpoints/Context.hpp"

namespace sdesktop::endpoints
{

    class OutboxHelper
    {
        sys::Service *ownerServicePtr = nullptr;

      public:
        explicit OutboxHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};

        void processGetRequest(Context &context);
        void processDeleteRequest(Context &context);

        static auto toJson(const Outbox::NotificationEntry &record) -> json11::Json;

      private:
        void getNotifications(Context &context);
        void removeNotifications(Context &context);
    };

} // namespace sdesktop::endpoints
