// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "BluetoothHelper.hpp"

#include <memory>
#include <string>
namespace sys
{
    class Service;
} // namespace sys

namespace sdesktop::endpoints
{

    class BluetoothEndpoint : public Endpoint
    {
      private:
        std::unique_ptr<BluetoothHelper> helper;

      public:
        BluetoothEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
        {
            debugName = "BluetoothEndpoint";
            helper    = std::make_unique<BluetoothHelper>(ownerServicePtr);
        }
        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
