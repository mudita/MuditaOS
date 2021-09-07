// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "SecurityEndpointHelper.hpp"

namespace parserFSM
{
    class Context;
} // namespace parserFSM

namespace sys
{
    class Service;
} // namespace sys

enum class EndpointSecurity
{
    Allow,
    Block
};

class SecurityEndpoint : public parserFSM::Endpoint
{
    const std::unique_ptr<parserFSM::SecurityEndpointHelper> helper;

  public:
    explicit SecurityEndpoint(sys::Service *ownerServicePtr)
        : Endpoint(ownerServicePtr), helper(std::make_unique<parserFSM::SecurityEndpointHelper>(ownerServicePtr))
    {
        debugName = "SecurityEndpoint";
    }

    auto handle(parserFSM::Context &context) -> void override;
};
