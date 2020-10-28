// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationManifest.hpp"

#include <algorithm>

namespace app::manager
{
    ApplicationManifest::ApplicationManifest(actions::ActionFilter _actions) : actions{std::move(_actions)}
    {}

    auto ApplicationManifest::contains(actions::ActionId action) const noexcept -> bool
    {
        auto it = std::find(actions.begin(), actions.end(), action);
        return it != actions.end();
    }
} // namespace app::manager
