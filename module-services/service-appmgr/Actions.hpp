// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <module-gui/gui/SwitchData.hpp>

namespace app::manager::actions
{
    using ActionId     = int;
    using ActionFilter = std::vector<ActionId>;

    enum Action : ActionId
    {
        Launch,
        Call,
        UserAction
    };

    using ActionParams    = gui::SwitchData;
    using ActionParamsPtr = std::unique_ptr<ActionParams>;
} // namespace app::manager::actions
