// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractController.hpp"
#include "CommandHandler.hpp"
#include "interface/BluetoothDriver.hpp"
#include <command/event/Events.hpp>

#include <functional>
#include <memory>

namespace bluetooth
{
    using DeviceRegistrationFunction = std::function<Error::Code()>;

    class StatefulController : public AbstractController
    {
      public:
        StatefulController(std::shared_ptr<AbstractDriver> driver,
                           std::shared_ptr<AbstractCommandHandler> handler,
                           DeviceRegistrationFunction registerDevice,
                           std::shared_ptr<bluetooth::SettingsHolder> settings,
                           std::shared_ptr<std::vector<Devicei>> pairedDevices,
                           std::shared_ptr<bluetooth::BaseProfileManager> profileManager);
        StatefulController()                                = delete;
        StatefulController(const StatefulController &other) = delete;
        StatefulController(StatefulController &&other) noexcept;
        StatefulController &operator=(const StatefulController &other) = delete;
        StatefulController &operator                                   =(StatefulController &&other) noexcept;

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

        class Impl;

      private:
        std::unique_ptr<Impl> pimpl;
    };
} // namespace bluetooth
