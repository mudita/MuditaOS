// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-services/service-desktop/endpoints/Endpoint.hpp>
#include "Service/Service.hpp"

class FilesystemEndpoint : public parserFSM::Endpoint
{
  public:
    FilesystemEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {}
    auto handle(parserFSM::Context &context) -> void override;
    auto run(parserFSM::Context &context) -> sys::ReturnCodes;
    auto getUpdates(parserFSM::Context &context) -> sys::ReturnCodes;
};
