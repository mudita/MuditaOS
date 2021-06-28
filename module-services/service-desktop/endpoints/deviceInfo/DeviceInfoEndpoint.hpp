// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DeviceInfoEndpointHelper.hpp"
#include <endpoints/Endpoint.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class DeviceInfoEndpoint : public parserFSM::Endpoint
{
    const std::unique_ptr<parserFSM::DeviceInfoEndpointHelper> helper;

  public:
    explicit DeviceInfoEndpoint(sys::Service *ownerServicePtr)
        : Endpoint(ownerServicePtr), helper(std::make_unique<parserFSM::DeviceInfoEndpointHelper>(ownerServicePtr))
    {
        debugName = "DeviceInfoEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;
};
