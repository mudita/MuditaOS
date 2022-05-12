// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CommandHandler.hpp"
#include "interface/BluetoothDriver.hpp"
#include <command/event/Events.hpp>

#include <functional>
#include <memory>

namespace bluetooth
{
    using DeviceRegistrationFunction = std::function<Error::Code()>;

    class AbstractController
    {
      public:
        virtual ~AbstractController() noexcept = default;

        virtual void handle(const bt::evt::Base &evt) = 0;
        virtual void handle(const bt::evt::StartScan &evt)           = 0;
        virtual void handle(const bt::evt::StopScan &evt)            = 0;
        virtual void handle(const bt::evt::GetDevicesAvailable &evt) = 0;
        virtual void handle(const bt::evt::VisibilityOn &evt)        = 0;
        virtual void handle(const bt::evt::VisibilityOff &evt)       = 0;
        virtual void handle(const bt::evt::ConnectAudio &evt)        = 0;
        virtual void handle(const bt::evt::DisconnectAudio &evt)     = 0;
        virtual void handle(const bt::evt::PowerOn &evt)             = 0;
        virtual void handle(const bt::evt::PowerOff &evt)            = 0;
        virtual void handle(const bt::evt::ShutDown &evt)            = 0;
        virtual void handle(const bt::evt::Pair &evt)                = 0;
        virtual void handle(const bt::evt::Unpair &evt)              = 0;
        virtual void handle(const bt::evt::StartRinging &evt)        = 0;
        virtual void handle(const bt::evt::StopRinging &evt)         = 0;
        virtual void handle(const bt::evt::StartRouting &evt)        = 0;
        virtual void handle(const bt::evt::StartStream &evt)         = 0;
        virtual void handle(const bt::evt::StopStream &evt)          = 0;
        virtual void handle(const bt::evt::CallAnswered &evt)        = 0;
        virtual void handle(const bt::evt::CallTerminated &evt)      = 0;
        virtual void handle(const bt::evt::CallStarted &evt)         = 0;
        virtual void handle(const bt::evt::IncomingCallNumber &evt)  = 0;
        virtual void handle(const bt::evt::SignalStrengthData &evt)  = 0;
        virtual void handle(const bt::evt::OperatorNameData &evt)    = 0;
        virtual void handle(const bt::evt::BatteryLevelData &evt)    = 0;
        virtual void handle(const bt::evt::NetworkStatusData &evt)   = 0;

        virtual auto isTerminated() const -> bool = 0;
    };

    class StatefulController : public AbstractController
    {
      public:
        StatefulController(std::shared_ptr<AbstractDriver> &&driver,
                           std::shared_ptr<AbstractCommandHandler> &&handler,
                           DeviceRegistrationFunction &&registerDevice,
                           std::shared_ptr<bluetooth::SettingsHolder> &&settings,
                           std::shared_ptr<std::vector<Devicei>> &&pairedDevices);

        ~StatefulController() noexcept override;

        void handle(const bt::evt::Base &evt) override;
        void handle(const bt::evt::StartScan &evt) override;
        void handle(const bt::evt::StopScan &evt) override;
        void handle(const bt::evt::GetDevicesAvailable &evt) override;
        void handle(const bt::evt::VisibilityOn &evt) override;
        void handle(const bt::evt::VisibilityOff &evt) override;
        void handle(const bt::evt::ConnectAudio &evt) override;
        void handle(const bt::evt::DisconnectAudio &evt) override;
        void handle(const bt::evt::PowerOn &evt) override;
        void handle(const bt::evt::PowerOff &evt) override;
        void handle(const bt::evt::ShutDown &evt) override;
        void handle(const bt::evt::Pair &evt) override;
        void handle(const bt::evt::Unpair &evt) override;
        void handle(const bt::evt::StartRinging &evt) override;
        void handle(const bt::evt::StopRinging &evt) override;
        void handle(const bt::evt::StartRouting &evt) override;
        void handle(const bt::evt::StartStream &evt) override;
        void handle(const bt::evt::StopStream &evt) override;
        void handle(const bt::evt::CallAnswered &evt) override;
        void handle(const bt::evt::CallTerminated &evt) override;
        void handle(const bt::evt::CallStarted &evt) override;
        void handle(const bt::evt::IncomingCallNumber &evt) override;
        void handle(const bt::evt::SignalStrengthData &evt) override;
        void handle(const bt::evt::OperatorNameData &evt) override;
        void handle(const bt::evt::BatteryLevelData &evt) override;
        void handle(const bt::evt::NetworkStatusData &evt) override;

        [[nodiscard]] auto isTerminated() const -> bool override;

      private:
        class Impl;
        std::unique_ptr<Impl> pimpl;
    };
} // namespace bluetooth
