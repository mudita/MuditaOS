// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace app::manager
{
    enum class Reaction
    {
        None,
        KeepFocusedAppInBackground
    };

    class OnActionPolicy
    {
      public:
        Reaction operator()(actions::Action action) const noexcept;
    };
} // namespace app::manager
