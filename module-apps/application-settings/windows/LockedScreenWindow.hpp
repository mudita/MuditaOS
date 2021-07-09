// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class LockedScreenWindow : public BaseSettingsWindow
    {
      public:
        LockedScreenWindow(app::Application *app);
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
