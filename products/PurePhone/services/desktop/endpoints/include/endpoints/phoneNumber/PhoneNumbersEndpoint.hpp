// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PhoneNumberHelper.hpp"
#include <endpoints/Endpoint.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    class PhoneNumbersEndpoint : public Endpoint
    {
      private:
        std::shared_ptr<PhoneNumberHelper> helper;

      public:
        explicit PhoneNumbersEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "PhoneNumbersEndpoint";
            helper    = std::make_shared<PhoneNumberHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
