// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "RebootHelper.hpp"

namespace sdesktop::endpoints
{
    class RebootEndpoint : public Endpoint
    {
      public:
        explicit RebootEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<RebootHelper>(ownerServicePtr))
        {
            debugName = "RebootEndpoint";
        }

        auto handle(Context &context) -> void override;

      private:
        const std::unique_ptr<RebootHelper> helper;
    };

} // namespace sdesktop::endpoints
