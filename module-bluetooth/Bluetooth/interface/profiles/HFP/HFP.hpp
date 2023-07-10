// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <interface/profiles/PhoneInterface.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <btstack_run_loop.h>
#include <PhoneNumber.hpp>

namespace bluetooth
{

    class HFP : public CallProfile
    {
        // static constexpr auto CLASS_OF_DEVICE = 0x400204;
        // Service class: Telephony, Major device class: Phone, Minor device class: Cellular
      public:
        HFP();
        ~HFP() override;

        HFP(const HFP &other) = delete;
        auto operator=(const HFP &rhs) -> HFP & = delete;
        HFP(HFP &&other) noexcept;
        auto operator=(HFP &&other) noexcept -> HFP &;

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
        /// @brief Sets the signal strength bars data
        /// @return Success
        [[nodiscard]] auto setSignalStrength(int bars) const noexcept -> Result::Code override;
        [[nodiscard]] auto setOperatorName(const std::string_view &name) const noexcept -> Result::Code override;
        [[nodiscard]] auto setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code override;
        [[nodiscard]] auto setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code override;
        auto setRoamingStatus(bool enabled) const noexcept -> Result::Code override;

        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) override;

      private:
        class HFPImpl;
        std::unique_ptr<HFPImpl> pimpl;
    };

} // namespace bluetooth
