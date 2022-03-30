// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CommandHandler.hpp"
#include "interface/BluetoothDriver.hpp"

#include <functional>
#include <memory>

namespace bluetooth
{
    using DeviceRegistrationFunction = std::function<Error::Code()>;

    class AbstractController
    {
      public:
        virtual ~AbstractController() noexcept = default;

        virtual void turnOn()                     = 0;
        virtual void turnOff()                    = 0;
        virtual void shutdown()                   = 0;
        virtual auto isOn() const -> bool         = 0;
        virtual auto isTerminated() const -> bool = 0;

        virtual void processCommand(Command &command) = 0;
    };

    class StatefulController : public AbstractController
    {
      public:
        StatefulController(std::shared_ptr<AbstractDriver> &&driver,
                           std::shared_ptr<AbstractCommandHandler> &&handler,
                           DeviceRegistrationFunction &&registerDevice);
        ~StatefulController() noexcept override;

        void turnOn() override;
        void turnOff() override;
        void shutdown() override;
        [[nodiscard]] auto isOn() const -> bool override;
        [[nodiscard]] auto isTerminated() const -> bool override;

        void processCommand(Command &command) override;

      private:
        class Impl;
        std::unique_ptr<Impl> pimpl;
    };
} // namespace bluetooth
