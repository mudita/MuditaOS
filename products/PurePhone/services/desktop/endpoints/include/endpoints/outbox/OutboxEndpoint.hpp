// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OutboxHelper.hpp"
#include <endpoints/Endpoint.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    class OutboxEndpoint : public Endpoint
    {
      private:
        std::shared_ptr<OutboxHelper> helper;

      public:
        explicit OutboxEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "OutboxEndpoint";
            helper    = std::make_shared<OutboxHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
