// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include <Service/Message.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
#include <memory>
namespace bluetooth
{
    class Profile
    {
      public:
        virtual ~Profile()                                                   = default;
        virtual auto init() -> Error::Code                                   = 0;
        virtual void setDeviceAddress(uint8_t *addr)                         = 0;
        virtual void setOwnerService(const sys::Service *service)            = 0;
        virtual auto getStreamData() -> std::shared_ptr<BluetoothStreamData> = 0;
        virtual void connect()                                               = 0;
        virtual void start()                                                 = 0;
        virtual void stop()                                                  = 0;
        virtual void disconnect()                                            = 0;
    };

} // namespace bluetooth
