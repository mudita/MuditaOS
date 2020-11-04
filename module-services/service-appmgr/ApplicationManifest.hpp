// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Actions.hpp"

namespace app::manager
{
    struct ApplicationManifest
    {
        ApplicationManifest(actions::ActionFilter _actions = {});

        auto contains(actions::ActionId action) const noexcept -> bool;

        /// Actions the application can respond to.
        actions::ActionFilter actions;
    };
} // namespace app::manager
