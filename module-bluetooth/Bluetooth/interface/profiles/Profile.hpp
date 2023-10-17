// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <audio/BluetoothAudioDevice.hpp>
#include <Service/Message.hpp>

#include <memory>
#include <command/BatteryLevel.hpp>
#include <Device.hpp>

namespace bluetooth
{
    class Profile
    {
      public:
        virtual ~Profile()                                                                        = default;
        virtual auto init() -> Result::Code                                                       = 0;
        virtual void setDevice(const Devicei &device)                                             = 0;
        virtual void setOwnerService(sys::Service *service)                                       = 0;
        virtual void connect()                                                                    = 0;
        virtual void disconnect()                                                                 = 0;
        virtual void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) = 0;

      protected:
        static void initSdp();
        static void initL2cap();

      private:
        static bool isL2CapInitialized;
        static bool isSdpInitialized;
    };

    class MusicProfile : public Profile
    {
      public:
        virtual void start() = 0;
        virtual void stop()  = 0;
    };

    class CallProfile : public Profile
    {
      public:
        /// Executed after incoming call has been received
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto incomingCallStarted() const noexcept -> Result::Code = 0;
        /// Executed after outgoing call has been placed
        /// @param outgoing call number
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto outgoingCallStarted(const std::string &number) const noexcept -> Result::Code = 0;
        /// Executed after incoming call is answered
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto incomingCallAnswered() const noexcept -> Result::Code = 0;
        /// Executed after outgoing call is answered
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto outgoingCallAnswered() const noexcept -> Result::Code = 0;
        /// Executed after termination of the ongoing call
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto callTerminated() const noexcept -> Result::Code = 0;
        /// Executed after termination of the incoming call that has not yet been answered
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto callMissed() const noexcept -> Result::Code = 0;
        /// Sets the incoming call number
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setIncomingCallNumber(const std::string &num) const noexcept -> Result::Code = 0;
        /// Sets the signal strength bars data in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setSignalStrength(int bars) const noexcept -> Result::Code = 0;
        /// Sets the operator name in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setOperatorName(const std::string_view &name) const noexcept -> Result::Code = 0;
        /// Sets the operator name in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code = 0;
        /// Sets the network registration status in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code = 0;
        /// Sets the roaming status in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        virtual auto setRoamingStatus(bool enabled) const noexcept -> Result::Code = 0;
    };
} // namespace bluetooth
