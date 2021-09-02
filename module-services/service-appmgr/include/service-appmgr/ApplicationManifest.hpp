// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

#include <apps-common/locks/handlers/LockPolicyHandler.hpp>

#include <optional>

namespace app::manager
{
    struct ApplicationManifest
    {
        using AutoLockPolicy = locks::AutoLockPolicy;
        ApplicationManifest(actions::ActionFilter _actions        = {},
                            AutoLockPolicy _startupAutoLockPolicy = AutoLockPolicy::DetermineByWindow);

        [[nodiscard]] auto contains(actions::ActionId action) const noexcept -> bool;
        [[nodiscard]] auto getActionFlag(actions::ActionId action) const noexcept -> actions::ActionFlag;
        [[nodiscard]] auto getAutoLockPolicy() const noexcept -> AutoLockPolicy;

      private:
        /// Actions the application can respond to.
        actions::ActionFilter actions;
        AutoLockPolicy startupAutoLockPolicy = AutoLockPolicy::DetermineByWindow;

      private:
        auto find(actions::ActionId action) const noexcept -> std::optional<actions::RegisteredAction>;
    };
} // namespace app::manager
