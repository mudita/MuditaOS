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
    class USBSecurityModel
    {
      public:
        enum PhoneLockState : bool
        {
            Unlocked = false,
            Locked
        };
        explicit USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings);
        auto isPasscodeEnabled() const -> bool;
        auto setPhoneLocked() -> void;
        auto setPhoneUnlocked() -> void;
        auto isPhoneLocked() const -> bool;
        auto isSecurityEnabled() const -> bool;
        auto getEndpointSecurity() const -> EndpointSecurity;
        auto updatePhoneLockTime(const time_t newPhoneLockTime) -> void;
        auto getPhoneLockTime() const -> time_t;

      private:
        PhoneLockState phoneLocked;
        settings::Settings *settings;
        time_t phoneLockTime = 0;
    };
}; // namespace sdesktop
