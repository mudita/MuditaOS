// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnActionPolicy.hpp"

namespace app::manager
{
    Reaction OnActionPolicy::operator()(actions::Action action) const noexcept
    {
        if (action == actions::HandleIncomingCall) {
            return Reaction::KeepFocusedAppInBackground;
        }
        return Reaction::None;
    }
} // namespace app::manager
