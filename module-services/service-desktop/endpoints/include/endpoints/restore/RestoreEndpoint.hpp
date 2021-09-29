// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{

    class RestoreEndpoint : public Endpoint
    {
      public:
        explicit RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "RestoreEndpoint";
        }
        auto handle(Context &context) -> void override;

      private:
        auto handleGet(Context &context) -> sys::ReturnCodes;
        auto executeRestore(Context &context) -> sys::ReturnCodes;
    };

} // namespace sdesktop::endpoints
