// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

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
