// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include "interface/profiles/ProfileManager.hpp"
#include "interface/BluetoothDriver.hpp"

#include <cstdint>
#include <PhoneNumber.hpp>

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
            CallAnswered,
            IncomingCallNumber,
            None,
        };
        Command(Command::Type type, std::optional<Devicei> dev) : type(type)
        {
            if (dev.has_value()) {
                device = dev.value();
            }
        }
        Command(Command::Type type, std::optional<utils::PhoneNumber::View> num) : type(type)
        {
            if (num.has_value()) {
                numberStringPtr  = new std::string();
                *numberStringPtr = num->getEntered();
            }
        }
        explicit Command(Command::Type type) : type(type)
        {}
        auto getType() const noexcept -> Command::Type
        {
            return type;
        }

        auto getDevice() const noexcept -> Devicei
        {
            return device;
        }

        auto getNumberString() noexcept -> std::string
        {
            if (numberStringPtr != nullptr) {
                return *numberStringPtr;
            }
            return std::string();
        }

        void destroy()
        {
            delete numberStringPtr;
        }

      private:
        Devicei device{};
        std::string *numberStringPtr = nullptr;
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
