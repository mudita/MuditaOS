// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "SecurityEndpointHelper.hpp"

namespace sdesktop::endpoints
{

    enum class EndpointSecurity
    {
        Allow,
        Block
    };

    class SecurityEndpoint : public Endpoint
    {
        const std::unique_ptr<SecurityEndpointHelper> helper;

      public:
        explicit SecurityEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<SecurityEndpointHelper>(ownerServicePtr))
        {
            debugName = "SecurityEndpoint";
        }

        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
