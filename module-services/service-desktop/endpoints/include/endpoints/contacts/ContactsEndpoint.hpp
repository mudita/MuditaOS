// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ContactHelper.hpp"

#include <endpoints/Endpoint.hpp>

#include <Service/Service.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    class ContactsEndpoint : public Endpoint
    {
      private:
        std::unique_ptr<ContactHelper> helper;

      public:
        explicit ContactsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "ContactsEndpoint";
            helper    = std::make_unique<ContactHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
