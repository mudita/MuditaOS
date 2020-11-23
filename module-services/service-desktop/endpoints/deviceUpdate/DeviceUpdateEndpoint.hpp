// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <Service/Service.hpp>

#include <memory>
#include <string>

namespace parserFSM
{
    class Context;

    namespace json::deviceUpdate
    {
        inline constexpr auto simSwitch    = "simSwitch";
    }
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class DeviceUpdateEndpoint : public Endpoint
{

  public:
    DeviceUpdateEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "DeviceUpdateEndpoint";
    }

    auto handle(Context &context) -> void override;
    auto processPutRequest(Context &context) -> sys::ReturnCodes;
};
