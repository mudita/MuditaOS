// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/Actions.hpp"
#include "service-appmgr/ApplicationManifest.hpp"

#include <algorithm>

namespace app::manager
{
    ApplicationManifest::ApplicationManifest(actions::ActionFilter _actions, AutoLockPolicy _startupAutoLockPolicy)
        : actions{std::move(_actions)}, startupAutoLockPolicy{_startupAutoLockPolicy}
    {}

    auto ApplicationManifest::contains(actions::ActionId action) const noexcept -> bool
    {
        auto it = std::find(actions.begin(), actions.end(), action);
        return it != actions.end();
    }

    [[nodiscard]] auto ApplicationManifest::getAutoLockPolicy() const noexcept -> AutoLockPolicy
    {
        return startupAutoLockPolicy;
    }
} // namespace app::manager
