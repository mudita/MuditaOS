// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <Service/Common.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class BackupEndpoint : public Endpoint
{
  public:
    BackupEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "BackupEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto request(Context &context) -> sys::ReturnCodes;
    auto upload(Context &context) -> sys::ReturnCodes;
};
