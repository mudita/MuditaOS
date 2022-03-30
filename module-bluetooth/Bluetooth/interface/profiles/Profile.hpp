// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include <audio/BluetoothAudioDevice.hpp>
#include <Service/Message.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>

#include <memory>

namespace bluetooth
{
    class Profile
    {
      public:
        virtual ~Profile()                                                                        = default;
        virtual auto init() -> Error::Code                                                        = 0;
        virtual void setDevice(const Devicei &device)                                             = 0;
        virtual void setOwnerService(const sys::Service *service)                                 = 0;
        virtual void connect()                                                                    = 0;
        virtual void start()                                                                      = 0;
        virtual void stop()                                                                       = 0;
        virtual void disconnect()                                                                 = 0;
        virtual void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) = 0;
        /// Starts ringing
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto startRinging() const noexcept -> Error::Code = 0;
        /// Stops ringing
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto stopRinging() const noexcept -> Error::Code = 0;
        /// Initializes call
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto initializeCall() const noexcept -> Error::Code = 0;
        /// Executed after the call is answered
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto callAnswered() const noexcept -> Error::Code = 0;
        /// Sets the incoming call number
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setIncomingCallNumber(const std::string &num) const noexcept -> Error::Code = 0;
        /// Sets the signal strength bars data in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setSignalStrength(int bars) const noexcept -> Error::Code = 0;
        /// Sets the operator name in HFP profile
        /// @return Error code that determines, whether operation was successful or not
        [[nodiscard]] virtual auto setOperatorName(const std::string_view &name) const noexcept -> Error::Code = 0;

      protected:
        static void initSdp();
        static void initL2cap();

      private:
        static bool isL2CapInitialized;
        static bool isSdpInitialized;
    };

} // namespace bluetooth
