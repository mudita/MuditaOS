// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/core/Context.hpp>
#include <gui/Common.hpp>

namespace service::eink
{
    struct ImageData
    {
        gui::Context context;
        gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_FAST;
    };
} // namespace service::eink
