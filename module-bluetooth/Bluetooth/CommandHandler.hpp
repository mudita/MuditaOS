// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
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
}

namespace bluetooth
{
    class SettingsHolder;

    class AbstractCommandHandler
    {
      public:
        virtual ~AbstractCommandHandler() noexcept = default;

        virtual Error::Code scan()                                            = 0;
        virtual Error::Code stopScan()                                        = 0;
        virtual Error::Code setVisibility(bool visibility)                    = 0;
        virtual Error::Code connect(const DataVariant &data)                  = 0;
        virtual Error::Code disconnect()                                      = 0;
        virtual Error::Code pair(const DataVariant &data)                     = 0;
        virtual Error::Code unpair(const DataVariant &data)                   = 0;
        virtual Error::Code availableDevices()                                = 0;
    };

    class CommandHandler : public AbstractCommandHandler
    {
      public:
        explicit CommandHandler(sys::Service *service,
                                std::shared_ptr<bluetooth::SettingsHolder> settings,
                                std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                                std::shared_ptr<bluetooth::AbstractDriver> driver);

        Error::Code scan() override;
        Error::Code stopScan() override;
        Error::Code setVisibility(bool visibility) override;
        Error::Code connect(const DataVariant &data) override;
        Error::Code disconnect() override;
        Error::Code pair(const DataVariant &data) override;
        Error::Code unpair(const DataVariant &data) override;
        Error::Code availableDevices() override;

      private:
        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;

      public:
        std::shared_ptr<bluetooth::BaseProfileManager> profileManager;

      private:
        std::shared_ptr<AbstractDriver> driver;
    };
} // namespace bluetooth
