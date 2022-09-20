// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

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
    using endpoints::BlockReason;
    using endpoints::EndpointSecurity;

    class USBSecurityModel
    {
      public:
        enum PhoneLockState : bool
        {
            Unlocked = false,
            Locked
        };
        typedef struct
        {
            EndpointSecurity access;
            BlockReason reason;
        } endpointSecurity_t;

        explicit USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings);

        auto setPhoneLocked() -> void;
        auto setPhoneUnlocked() -> void;
        auto isSecurityEnabled() const -> bool;
        auto getEndpointSecurity() const -> endpointSecurity_t;
        auto updatePhoneLockTime(const time_t newPhoneLockTime) -> void;
        auto getPhoneLockTime() const -> time_t;

      private:
        auto isPasscodeEnabled() const -> bool;
        auto isPhoneLocked() const -> bool;
        auto isEulaAccepted() const -> bool;

        PhoneLockState phoneLocked;
        settings::Settings *settings;
        time_t phoneLockTime = 0;
    };
}; // namespace sdesktop
