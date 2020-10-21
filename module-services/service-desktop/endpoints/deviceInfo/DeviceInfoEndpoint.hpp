// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"    // for Endpoint
#include "ParserUtils.hpp" // for parserFSM

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

class DeviceInfoEndpoint : public Endpoint
{

  public:
    DeviceInfoEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "DeviceInfoEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto getDeviceInfo(Context &context) -> bool;
};
