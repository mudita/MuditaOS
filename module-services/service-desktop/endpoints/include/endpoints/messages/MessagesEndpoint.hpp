// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MessageHelper.hpp"
#include <endpoints/Endpoint.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    class MessagesEndpoint : public Endpoint
    {
      private:
        std::shared_ptr<MessageHelper> helper;

      public:
        explicit MessagesEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "MessagesEndpoint";
            helper    = std::make_shared<MessageHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
