// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-appmgr/Actions.hpp"
#include <locks/handlers/LockPolicyHandler.hpp>

namespace app::manager
{
    struct ApplicationManifest
    {
        using AutoLockPolicy = locks::AutoLockPolicy;
        ApplicationManifest(actions::ActionFilter _actions        = {},
                            AutoLockPolicy _startupAutoLockPolicy = AutoLockPolicy::DetermineByWindow);

        [[nodiscard]] auto contains(actions::ActionId action) const noexcept -> bool;
        [[nodiscard]] auto getAutoLockPolicy() const noexcept -> AutoLockPolicy;

      private:
        /// Actions the application can respond to.
        actions::ActionFilter actions;
        AutoLockPolicy startupAutoLockPolicy = AutoLockPolicy::DetermineByWindow;
    };
} // namespace app::manager
