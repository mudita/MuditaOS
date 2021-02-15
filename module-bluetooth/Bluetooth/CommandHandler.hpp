// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"
#include "interface/profiles/Profile.hpp"
#include "interface/BluetoothDriver.hpp"

#include <service-bluetooth/SettingsHolder.hpp>
#include <Service/Service.hpp>

#include <cstdint>

namespace bluetooth
{
    enum Command : std::uint8_t
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
                                std::shared_ptr<bluetooth::Driver> driver);

        auto handle(Command command) -> Error::Code override;

      private:
        Error::Code scan();
        Error::Code stopScan();
        Error::Code startPan();
        Error::Code setVisibility(bool visibility);
        Error::Code establishAudioConnection();
        Error::Code disconnectAudioConnection();

        sys::Service *service;
        std::shared_ptr<bluetooth::SettingsHolder> settings;
        std::shared_ptr<bluetooth::Profile> currentProfile;
        std::shared_ptr<Driver> driver;
    };
} // namespace bluetooth
