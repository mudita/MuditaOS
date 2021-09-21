// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Context.hpp"

#include <Service/Service.hpp>

namespace sdesktop::endpoints
{

    enum class EndpointSecurity
    {
        Allow,
        Block
    };

    class Endpoint
    {
      public:
        explicit Endpoint(sys::Service *_ownerServicePtr);
        virtual ~Endpoint()                           = default;
        virtual auto handle(Context &context) -> void = 0;
        auto c_str() -> const char *;

      protected:
        std::string debugName;
        sys::Service *ownerServicePtr = nullptr;
    };

    class SecuredEndpoint : public Endpoint
    {
      public:
        explicit SecuredEndpoint(sys::Service *ownerServicePtr);
        ~SecuredEndpoint() = default;

        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
