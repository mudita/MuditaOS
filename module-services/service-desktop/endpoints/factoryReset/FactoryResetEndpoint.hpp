// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <Service/Service.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class FactoryResetEndpoint : public parserFSM::Endpoint
{

  public:
    FactoryResetEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "FactoryResetEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;
    auto makeFactoryReset(parserFSM::Context &context) -> bool;
};
