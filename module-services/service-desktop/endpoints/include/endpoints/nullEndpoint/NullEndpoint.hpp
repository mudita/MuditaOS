// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{

    class NullEndpoint : public Endpoint
    {
      public:
        explicit NullEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "NullEndpoint";
        }

        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
