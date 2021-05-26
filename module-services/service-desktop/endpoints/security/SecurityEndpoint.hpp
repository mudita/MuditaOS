// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>
#include "SecurityEndpointHelper.hpp"
#include <string>

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
    Allow = 0,
    Block = 1
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

  private:
    auto processHandshake(parserFSM::Context &context) -> parserFSM::http::Code;
    auto processConfiguration(parserFSM::Context &context) -> parserFSM::http::Code;
    auto processStatus(parserFSM::Context &context) -> parserFSM::http::Code;
};
