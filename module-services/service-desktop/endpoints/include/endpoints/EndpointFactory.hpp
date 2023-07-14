// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{
    class EndpointFactory
    {
      protected:
        static constexpr auto whitelist = {EndpointType::DeveloperMode, EndpointType::UsbSecurity};

        EndpointFactory() = default;

      public:
        virtual ~EndpointFactory() = default;

        static auto create(EndpointSecurity security = EndpointSecurity::Allow) -> std::unique_ptr<EndpointFactory>;

        virtual auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint> = 0;
    };
} // namespace sdesktop::endpoints
