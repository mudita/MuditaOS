// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "Profile.hpp"
#include "Service/Service.hpp"
#include "queue.h"
#include <memory>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
namespace bluetooth
{
    class A2DP : public Profile
    {
      public:
        A2DP();
        ~A2DP() override;

        A2DP(A2DP &other);
        auto operator=(A2DP rhs) -> A2DP &;
        A2DP(A2DP &&other) noexcept;
        auto operator=(A2DP &&other) noexcept -> A2DP &;

        auto init() -> Error::Code override;
        void setDeviceAddress(uint8_t *addr) override;
        void setOwnerService(const sys::Service *service) override;
        [[nodiscard]] auto getStreamData() -> std::shared_ptr<BluetoothStreamData> override;

        void connect() override;
        void disconnect() override;
        void start() override;
        void stop() override;

      private:
        class A2DPImpl;
        std::unique_ptr<A2DPImpl> pimpl;
    };
} // namespace Bt
