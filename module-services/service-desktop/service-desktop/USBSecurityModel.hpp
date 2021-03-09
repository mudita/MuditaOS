// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-desktop/endpoints/EndpointFactory.hpp>

namespace settings
{
    class Settings;
}

namespace sys
{
    class Service;
}

namespace sdesktop
{
    namespace usb
    {
        class USBHandshake;
    };

    using DeviceID = std::string;
    using Passcode = unsigned int;

    class USBSecurityModel
    {
      public:
        explicit USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings);

        auto isBound(DeviceID id) const -> bool;
        auto addDevice(DeviceID id, Passcode passcode) -> bool;
        bool checkPasscode(const Passcode &passcode);

        auto isSecurityEnabled() const -> bool;
        void enableEndpointSecurity(bool securityEnabled);

        bool processHandshake(const sdesktop::usb::USBHandshake *handshake);

        void setEndpointSecurity(EndpointSecurity security);

        auto getEndpointSecurity() const -> EndpointSecurity;

      private:
        std::set<DeviceID> parseDevices(const std::string &value) const;
        std::string dumpDevices(const std::set<DeviceID> &devices) const;

      private:
        unsigned int lockPassHash = 0;
        EndpointSecurity endpointSecurity;
        std::set<DeviceID> bounded;
        settings::Settings *settings;
    };

}; // namespace sdesktop