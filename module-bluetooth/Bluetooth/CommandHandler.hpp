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

        virtual auto handle(Command &command) -> Error::Code = 0;
    };

    class CommandHandler : public AbstractCommandHandler
    {
      public:
        explicit CommandHandler(sys::Service *service,
                                std::shared_ptr<bluetooth::SettingsHolder> settings,
                                std::shared_ptr<bluetooth::ProfileManager> profileManager,
                                std::shared_ptr<bluetooth::AbstractDriver> driver);

        auto handle(Command &command) -> Error::Code override;

      private:
        Error::Code scan();
        Error::Code stopScan();
        Error::Code setVisibility(bool visibility);
        Error::Code establishAudioConnection(const DataVariant &data);
        Error::Code disconnectAudioConnection();
        Error::Code pair(const DataVariant &data);
        Error::Code unpair(const DataVariant &data);
        Error::Code availableDevices();
        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;
        std::shared_ptr<bluetooth::ProfileManager> profileManager;
        std::shared_ptr<AbstractDriver> driver;
    };
} // namespace bluetooth
