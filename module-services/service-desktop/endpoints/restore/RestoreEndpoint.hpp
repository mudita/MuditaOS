// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class RestoreEndpoint : public parserFSM::Endpoint
{
  public:
    explicit RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "RestoreEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;
    auto request(parserFSM::Context &context) -> sys::ReturnCodes;
};
