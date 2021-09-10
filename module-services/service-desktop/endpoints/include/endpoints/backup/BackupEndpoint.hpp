// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

#include <Service/Common.hpp>

#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace sdesktop::endpoints
{
    class BackupEndpoint : public Endpoint
    {
      public:
        BackupEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "BackupEndpoint";
        }
        auto handle(Context &context) -> void override;
        auto request(Context &context) -> sys::ReturnCodes;
    };

} // namespace sdesktop::endpoints
