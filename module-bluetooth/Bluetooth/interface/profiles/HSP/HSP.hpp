﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <service-bluetooth/BluetoothMessage.hpp>
#include <btstack_run_loop.h>
#include <module-bluetooth/Bluetooth/interface/profiles/PhoneInterface.hpp>

namespace bluetooth
{
    class HSP : public Profile
    {
        static constexpr auto CLASS_OF_DEVICE = 0x400204;
        // Service class: Telephony, Major device class: Phone, Minor device class: Cellular
      public:
        HSP();
        ~HSP() override;

        HSP(const HSP &other) = delete;
        auto operator=(const HSP &rhs) -> HSP & = delete;
        HSP(HSP &&other) noexcept;
        auto operator=(HSP &&other) noexcept -> HSP &;

        auto init() -> Error::Code override;
        void setDevice(const Devicei &device) override;
        void setOwnerService(const sys::Service *service) override;

        void connect() override;
        void disconnect() override;
        void start() override;
        void stop() override;
        /// @brief Starts ring
        /// @return Success
        [[nodiscard]] auto startRinging() const noexcept -> Error::Code override;
        /// @brief Stops ring
        /// @return Success
        [[nodiscard]] auto stopRinging() const noexcept -> Error::Code override;
        /// @brief Initializes bluetooth audio call which is divided into two parts:
        /// - Ring stop
        /// - SCO link establishment
        /// @return Success
        [[nodiscard]] auto initializeCall() const noexcept -> Error::Code override;

        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) override;

      private:
        class HSPImpl;
        std::unique_ptr<HSPImpl> pimpl;
        const sys::Service *ownerService{};
        btstack_run_loop *runLoopInstance{};
    };

} // namespace bluetooth
