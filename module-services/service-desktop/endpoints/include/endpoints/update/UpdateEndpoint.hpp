// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "UpdateHelper.hpp"

namespace sdesktop::endpoints
{

    class UpdateEndpoint : public Endpoint
    {
      private:
        const std::unique_ptr<UpdateHelper> updateHelper;

      public:
        explicit UpdateEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), updateHelper(std::make_unique<UpdateHelper>(ownerServicePtr))
        {
            debugName = "UpdateEndpoint";
        }
        auto handle(Context &context) -> void override;

        [[nodiscard]] auto helperSwitcher(Context &ctx) -> BaseHelper &;
    };

} // namespace sdesktop::endpoints
