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

        void handle(const bluetooth::event::Base &evt) override;
        void handle(const bluetooth::event::StartScan &evt) override;
        void handle(const bluetooth::event::StopScan &evt) override;
        void handle(const bluetooth::event::GetDevicesAvailable &evt) override;
        void handle(const bluetooth::event::VisibilityOn &evt) override;
        void handle(const bluetooth::event::VisibilityOff &evt) override;
        void handle(const bluetooth::event::Connect &evt) override;
        void handle(const bluetooth::event::Disconnect &evt) override;
        void handle(const bluetooth::event::PowerOn &evt) override;
        void handle(const bluetooth::event::PowerOff &evt) override;
        void handle(const bluetooth::event::ShutDown &evt) override;
        void handle(const bluetooth::event::Pair &evt) override;
        void handle(const bluetooth::event::Unpair &evt) override;
        void handle(const bluetooth::event::StartRinging &evt) override;
        void handle(const bluetooth::event::StopRinging &evt) override;
        void handle(const bluetooth::event::StartRouting &evt) override;
        void handle(const bluetooth::event::StartStream &evt) override;
        void handle(const bluetooth::event::StopStream &evt) override;
        void handle(const bluetooth::event::CallAnswered &evt) override;
        void handle(const bluetooth::event::CallTerminated &evt) override;
        void handle(const bluetooth::event::CallStarted &evt) override;
        void handle(const bluetooth::event::IncomingCallNumber &evt) override;
        void handle(const bluetooth::event::SignalStrengthData &evt) override;
        void handle(const bluetooth::event::OperatorNameData &evt) override;
        void handle(const bluetooth::event::BatteryLevelData &evt) override;
        void handle(const bluetooth::event::NetworkStatusData &evt) override;

        class Impl;

      private:
        std::unique_ptr<Impl> pimpl;
    };
} // namespace bluetooth
