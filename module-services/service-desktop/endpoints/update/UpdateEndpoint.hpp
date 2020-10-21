// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"    // for Endpoint
#include "ParserUtils.hpp" // for parserFSM

#include "Service/Common.hpp" // for ReturnCodes
#include "Service/Service.hpp"
#include <string> // for string

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class UpdateEndpoint : public Endpoint
{

  public:
    UpdateEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "UpdateEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto run(Context &context) -> sys::ReturnCodes;
    auto getUpdates(Context &context) -> sys::ReturnCodes;
};
