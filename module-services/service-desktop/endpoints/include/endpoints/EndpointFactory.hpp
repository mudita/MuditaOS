// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/security/SecurityEndpoint.hpp>

namespace sdesktop::endpoints
{
    class Endpoint;

    class EndpointFactory
    {
      public:
        virtual ~EndpointFactory() = default;
        virtual auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>;
    };
    class SecuredEndpointFactory : public EndpointFactory
    {
        static constexpr auto Whitelist = {EndpointType::developerMode, EndpointType::usbSecurity};

      public:
        explicit SecuredEndpointFactory(EndpointSecurity security);

        auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint> override;

      private:
        EndpointSecurity endpointSecurity;
    };

} // namespace sdesktop::endpoints
