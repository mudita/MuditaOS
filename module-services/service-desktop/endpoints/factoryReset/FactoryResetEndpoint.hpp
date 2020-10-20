// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace parserFSM;

class FactoryResetEndpoint : public Endpoint
{

  public:
    FactoryResetEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "FactoryResetEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto makeFactoryReset(Context &context) -> bool;
};
