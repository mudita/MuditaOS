// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/EndpointFactory.hpp>

namespace sdesktop::endpoints
{
    class EndpointFactoryBell : public EndpointFactory
    {
      public:
        EndpointFactoryBell() = default;

        auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint> override;
    };
} // namespace sdesktop::endpoints
