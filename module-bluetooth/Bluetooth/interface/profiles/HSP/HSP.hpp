// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <service-bluetooth/BluetoothMessage.hpp>
#include <btstack_run_loop.h>
#include <module-bluetooth/Bluetooth/interface/profiles/PhoneInterface.hpp>

namespace bluetooth
{
    class HSP : public CallProfile
    {
      public:
        HSP();
        ~HSP() override;

        HSP(const HSP &other) = delete;
        auto operator=(const HSP &rhs) -> HSP & = delete;
        HSP(HSP &&other) noexcept;
        auto operator=(HSP &&other) noexcept -> HSP &;

        auto init() -> Result::Code override;
        void setDevice(const Devicei &device) override;
        void setOwnerService(sys::Service *service) override;

        void connect() override;
        void disconnect() override;
        [[nodiscard]] auto incomingCallStarted() const noexcept -> Result::Code override;
        [[nodiscard]] auto outgoingCallStarted(const std::string &number) const noexcept -> Result::Code override;
        [[nodiscard]] auto incomingCallAnswered() const noexcept -> Result::Code override;
        [[nodiscard]] auto outgoingCallAnswered() const noexcept -> Result::Code override;
        [[nodiscard]] auto callTerminated() const noexcept -> Result::Code override;
        [[nodiscard]] auto callMissed() const noexcept -> Result::Code override;
        [[nodiscard]] auto setIncomingCallNumber(const std::string &num) const noexcept -> Result::Code override;
        /// @return Success - ignoring in HSP
        [[nodiscard]] auto setSignalStrength(int bars) const noexcept -> Result::Code override;
        /// @return Success - ignoring in HSP
        [[nodiscard]] auto setOperatorName(const std::string_view &name) const noexcept -> Result::Code override;
        /// @return Success - ignoring in HSP
        [[nodiscard]] auto setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code override;
        /// @return Success - ignoring in HSP
        [[nodiscard]] auto setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code override;
        /// @return Success - ignoring in HSP
        auto setRoamingStatus(bool enabled) const noexcept -> Result::Code override;

        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) override;

      private:
        class HSPImpl;
        std::unique_ptr<HSPImpl> pimpl;
    };

} // namespace bluetooth
