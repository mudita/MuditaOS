// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include "interface/profiles/ProfileManager.hpp"
#include "interface/BluetoothDriver.hpp"

#include <cstdint>

namespace sys
{
    class Service;
    class Message;
}

namespace bluetooth
{
    class SettingsHolder;
    class Command
    {
      public:
        enum Type
        {
            StartScan,
            StopScan,
            getDevicesAvailable,
            VisibilityOn,
            VisibilityOff,
            ConnectAudio,
            DisconnectAudio,
            PowerOn,
            PowerOff,
            Pair,
            Unpair,
            StartRinging,
            StopRinging,
            StartRouting,
            StartStream,
            StopStream,
            SwitchProfile,
            None,
        };

        explicit Command(Command::Type type, std::optional<Devicei> dev = std::nullopt) : type(type)
        {
            if (dev.has_value()) {
                device = dev.value();
            }
        }

        auto getType() const noexcept -> Command::Type
        {
            return type;
        }

        auto getDevice() const noexcept -> Devicei
        {
            return device;
        }

      private:
        Devicei device{};
        Type type;
    };

    class AbstractCommandHandler
    {
      public:
        virtual ~AbstractCommandHandler() noexcept = default;

        virtual auto handle(Command command) -> Error::Code = 0;
    };

    class CommandHandler : public AbstractCommandHandler
    {
      public:
        explicit CommandHandler(sys::Service *service,
                                std::shared_ptr<bluetooth::SettingsHolder> settings,
                                std::shared_ptr<bluetooth::ProfileManager> profileManager,
                                std::shared_ptr<bluetooth::AbstractDriver> driver);

        auto handle(Command command) -> Error::Code override;

      private:
        Error::Code scan();
        Error::Code stopScan();
        Error::Code setVisibility(bool visibility);
        Error::Code establishAudioConnection(const Devicei &device);
        Error::Code disconnectAudioConnection();
        Error::Code pair(const Devicei &device);
        Error::Code unpair(const Devicei &device);
        Error::Code availableDevices();
        Error::Code switchAudioProfile();
        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;
        std::shared_ptr<bluetooth::ProfileManager> profileManager;
        std::shared_ptr<AbstractDriver> driver;
    };
} // namespace bluetooth
