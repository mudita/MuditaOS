// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include <module-services/service-bluetooth/ServiceBluetoothCommon.hpp>
#include <memory>
namespace Bt
{
    class Profile
    {
      public:
        virtual ~Profile()                           = default;
        virtual auto init() -> Error                 = 0;
        virtual void setDeviceAddress(uint8_t *addr) = 0;
        virtual auto getStreamData() -> std::shared_ptr<BluetoothStreamData> = 0;
    };

} // namespace Bt
