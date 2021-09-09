// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

namespace sys
{
    class Service;
} // namespace sys

class NullEndpoint : public parserFSM::Endpoint
{
  public:
    explicit NullEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "NullEndpoint";
    }

    auto handle(parserFSM::Context &context) -> void override;
};
