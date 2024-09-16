// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "TimeSyncHelper.hpp"
#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{
    class TimeSyncEndpoint : public Endpoint
    {
      public:
        explicit TimeSyncEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<TimeSyncHelper>(ownerServicePtr))
        {
            debugName = "TimeSyncEndpoint";
        }

        auto handle(Context &context) -> void override;

      private:
        const std::unique_ptr<TimeSyncHelper> helper;
    };
} // namespace sdesktop::endpoints
