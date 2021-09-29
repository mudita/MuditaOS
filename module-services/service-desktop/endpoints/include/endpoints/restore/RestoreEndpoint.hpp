// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RestoreHelper.hpp"
#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{

    class RestoreEndpoint : public Endpoint
    {
      public:
        explicit RestoreEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<RestoreHelper>(ownerServicePtr))
        {
            debugName = "RestoreEndpoint";
        }
        auto handle(Context &context) -> void override;

      private:
        const std::unique_ptr<RestoreHelper> helper;
    };

} // namespace sdesktop::endpoints
