// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include "interface/profiles/ProfileManager.hpp"
#include "interface/BluetoothDriver.hpp"

#include <cstdint>
#include <PhoneNumber.hpp>
#include <command/Command.hpp>
#include <utility>

namespace sys
{
    class Service;
    class Message;
} // namespace sys

namespace bluetooth
{
    class SettingsHolder;

    class AbstractCommandHandler
    {
      public:
        virtual ~AbstractCommandHandler() noexcept = default;

        virtual Result::Code scan()                           = 0;
        virtual Result::Code stopScan()                       = 0;
        virtual Result::Code setVisibility(bool visibility)   = 0;
        virtual Result::Code connect(const DataVariant &data) = 0;
        virtual Result::Code disconnect()                     = 0;
        virtual Result::Code pair(const DataVariant &data)    = 0;
        virtual Result::Code unpair(const DataVariant &data)  = 0;
        virtual Result::Code availableDevices()               = 0;
    };

    class CommandHandler : public AbstractCommandHandler
    {
      public:
        explicit CommandHandler(sys::Service *service,
                                std::shared_ptr<bluetooth::SettingsHolder> settings,
                                std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                                std::shared_ptr<bluetooth::AbstractDriver> driver);

        Result::Code scan() override;
        Result::Code stopScan() override;
        Result::Code setVisibility(bool visibility) override;
        Result::Code connect(const DataVariant &data) override;
        Result::Code disconnect() override;
        Result::Code pair(const DataVariant &data) override;
        Result::Code unpair(const DataVariant &data) override;
        Result::Code availableDevices() override;

      private:
        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;

      public:
        std::shared_ptr<bluetooth::BaseProfileManager> profileManager;

      private:
        std::shared_ptr<AbstractDriver> driver;
    };
} // namespace bluetooth
