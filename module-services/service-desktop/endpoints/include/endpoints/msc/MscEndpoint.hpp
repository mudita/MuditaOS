// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{

    class MscEndpoint : public Endpoint
    {
      public:
        explicit MscEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "MscEndpoint";
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
