// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/EndpointFactory.hpp>

namespace sdesktop::endpoints
{

    class EndpointFactoryBell : public EndpointFactory
    {
      public:
        explicit EndpointFactoryBell();

        virtual auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint> override;
    };

} // namespace sdesktop::endpoints
