﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/USBSecurityModel.hpp"

#include "Service/Service.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include <EventStore.hpp>

#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace sdesktop
{
    USBSecurityModel::USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings)
    {
        settings    = srvSettings;
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

    auto USBSecurityModel::isOnboardingFinished() const -> bool
    {
        return utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::onboardingDone, settings::SettingsScope::Global));
    }

    auto USBSecurityModel::isBatteryLevelCritical() const -> bool
    {
        using LevelState = Store::Battery::LevelState;

        return Store::Battery::get().levelState == LevelState::CriticalCharging or
               Store::Battery::get().levelState == LevelState::CriticalNotCharging;
    }

    auto USBSecurityModel::getEndpointSecurity() const -> endpointSecurity_t
    {
        if (isBatteryLevelCritical()) {
            return {EndpointSecurity::Block, BlockReason::BatteryCriticalLevel};
        }
        if (!isOnboardingFinished()) {
            return {EndpointSecurity::Block, BlockReason::OnboardingNotFinished};
        }
        if (isSecurityEnabled()) {
            return {EndpointSecurity::Block, BlockReason::DeviceLocked};
        }
        return {EndpointSecurity::Allow, BlockReason::NoReason};
    }

    auto USBSecurityModel::updatePhoneLockTime(const time_t newPhoneLockTime) -> void
    {
        phoneLockTime = newPhoneLockTime;
    }

    auto USBSecurityModel::getPhoneLockTime() const -> time_t
    {
        return phoneLockTime;
    }
} // namespace sdesktop
