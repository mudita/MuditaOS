// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "BackupHelper.hpp"

namespace sdesktop::endpoints
{
    class BackupEndpoint : public Endpoint
    {
      public:
        explicit BackupEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<BackupHelper>(ownerServicePtr))
        {
            debugName = "BackupEndpoint";
        }

        auto handle(Context &context) -> void override;

      private:
        const std::unique_ptr<BackupHelper> helper;
    };

} // namespace sdesktop::endpoints
