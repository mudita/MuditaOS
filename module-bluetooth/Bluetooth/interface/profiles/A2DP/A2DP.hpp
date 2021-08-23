// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "Profile.hpp"
#include "Service/Service.hpp"
#include "queue.h"
#include <memory>

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
        void setDevice(const Devicei &device) override;
        void setOwnerService(const sys::Service *service) override;

        void connect() override;
        void disconnect() override;
        void start() override;
        void stop() override;
        /// @return SystemError - it's not posible to start ringing while there's A2DP active
        [[nodiscard]] auto startRinging() const noexcept -> Error::Code override;
        /// @return SystemError - it's not posible to stop ringing while there's A2DP active
        [[nodiscard]] auto stopRinging() const noexcept -> Error::Code override;
        /// @return SystemError - it's not posible to start routing while there's A2DP active
        [[nodiscard]] auto initializeCall() const noexcept -> Error::Code override;

        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) override;

      private:
        class A2DPImpl;
        std::unique_ptr<A2DPImpl> pimpl;
    };
} // namespace bluetooth
