// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogHelper.hpp"
#include <Service/Service.hpp>
#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{

    class CalllogEndpoint : public Endpoint
    {
      private:
        std::unique_ptr<CalllogHelper> helper;

      public:
        explicit CalllogEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "CalllogEndpoint";
            helper    = std::make_unique<CalllogHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
