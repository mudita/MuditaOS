// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/EndpointFactory.hpp>

namespace sdesktop::endpoints
{

    class EndpointFactoryPure : public EndpointFactory
    {
      public:
        explicit EndpointFactoryPure(EndpointSecurity security);

        virtual auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint> override;

      private:
        auto constructEndpoint(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>;

        EndpointSecurity endpointSecurity;
    };

} // namespace sdesktop::endpoints
