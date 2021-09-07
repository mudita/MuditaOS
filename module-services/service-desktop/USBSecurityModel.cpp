// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/USBSecurityModel.hpp"

#include "Service/Service.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/WorkerDesktop.hpp"

#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace sdesktop
{
    USBSecurityModel::USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings)
    {
        settings = srvSettings;
        phoneLocked = PhoneLockState::Locked;
    }

    auto USBSecurityModel::isPasscodeEnabled() const -> bool
    {
        return utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::lockScreenPasscodeIsOn, settings::SettingsScope::Global));
    }

    auto USBSecurityModel::setPhoneLocked() -> void
    {
        phoneLocked = PhoneLockState::Locked;
    }

    auto USBSecurityModel::setPhoneUnlocked() -> void
    {
        phoneLocked = PhoneLockState::Unlocked;
    }

    auto USBSecurityModel::isPhoneLocked() const -> bool
    {
        return phoneLocked == PhoneLockState::Locked;
    }

    auto USBSecurityModel::isSecurityEnabled() const -> bool
    {
        return isPasscodeEnabled() && isPhoneLocked();
    }

    auto USBSecurityModel::getEndpointSecurity() const -> EndpointSecurity
    {
        if (isSecurityEnabled()) {
            return EndpointSecurity::Block;
        }
        else {
            return EndpointSecurity::Allow;
        }
    }
} // namespace sdesktop