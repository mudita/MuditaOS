// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include "interface/profiles/Profile.hpp"
#include "interface/BluetoothDriver.hpp"

#include <cstdint>

namespace sys
{
    class Service;
}

namespace bluetooth
{
    class SettingsHolder;
    using CommandArgument = std::variant<std::string, bool, int>;
    class Command
    {
      public:
        enum Type : std::uint8_t
        {
            StartScan,
            StopScan,
            StartPan,
            VisibilityOn,
            VisibilityOff,
            ConnectAudio,
            DisconnectAudio,
            PowerOn,
            PowerOff,
            Pair,
            None,
        };

        Command(Command::Type type, std::optional<CommandArgument> arg = std::nullopt)
            : argument(std::move(arg)), type(type)
        {}

        auto getType() const noexcept -> Command::Type
        {
            return type;
        }

        auto getArgument() const -> std::optional<CommandArgument>
        {
            return argument;
        }

      private:
        std::optional<CommandArgument> argument;
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
                                std::shared_ptr<bluetooth::Profile> currentProfile,
                                std::shared_ptr<bluetooth::AbstractDriver> driver);

        auto handle(Command command) -> Error::Code override;

      private:
        Error::Code scan();
        Error::Code stopScan();
        Error::Code startPan();
        Error::Code setVisibility(bool visibility);
        Error::Code establishAudioConnection();
        Error::Code disconnectAudioConnection();
        Error::Code pair(CommandArgument arg);

        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;
        std::shared_ptr<bluetooth::Profile> currentProfile;
        std::shared_ptr<AbstractDriver> driver;
    };
} // namespace bluetooth
