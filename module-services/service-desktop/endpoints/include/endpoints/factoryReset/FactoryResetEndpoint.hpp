// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

#include <Service/Service.hpp>

namespace sdesktop::endpoints
{

    class FactoryResetEndpoint : public Endpoint
    {

      public:
        FactoryResetEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "FactoryResetEndpoint";
        }
        auto handle(Context &context) -> void override;
        auto makeFactoryReset(Context &context) -> bool;
    };

} // namespace sdesktop::endpoints
