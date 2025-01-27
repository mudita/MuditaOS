// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "QuotesHelper.hpp"
#include <endpoints/Endpoint.hpp>

namespace sdesktop::endpoints
{
    class QuotesEndpoint : public Endpoint
    {
      public:
        explicit QuotesEndpoint(sys::Service *ownerServicePtr);
        auto handle(Context &context) -> void override;

      private:
        std::unique_ptr<QuotesHelper> helper;
    };

} // namespace sdesktop::endpoints
