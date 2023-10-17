// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationManifest.hpp"
#include <service-appmgr/Actions.hpp>

#include <algorithm>

namespace app::manager
{
    ApplicationManifest::ApplicationManifest(actions::ActionFilter _actions, AutoLockPolicy _startupAutoLockPolicy)
        : actions{std::move(_actions)}, startupAutoLockPolicy{_startupAutoLockPolicy}
    {}

    auto ApplicationManifest::contains(actions::ActionId action) const noexcept -> bool
    {
        const auto &foundAction = find(action);
        return foundAction.has_value();
    }

    auto ApplicationManifest::getActionFlag(actions::ActionId action) const noexcept -> actions::ActionFlag
    {
        const auto &foundAction = find(action);
        if (!foundAction.has_value()) {
            return actions::ActionFlag::None;
        }

        return foundAction->flag;
    }

    auto ApplicationManifest::find(actions::ActionId action) const noexcept -> std::optional<actions::RegisteredAction>
    {
        auto it = std::find_if(actions.begin(), actions.end(), [action](const actions::RegisteredAction &act) {
            return action == act.id;
        });
        if (it == actions.end()) {
            return std::nullopt;
        }
        return *it;
    }

    [[nodiscard]] auto ApplicationManifest::getAutoLockPolicy() const noexcept -> AutoLockPolicy
    {
        return startupAutoLockPolicy;
    }
} // namespace app::manager
