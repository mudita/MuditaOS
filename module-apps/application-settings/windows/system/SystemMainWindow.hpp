// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class SystemMainWindow : public BaseSettingsWindow
    {
      public:
        explicit SystemMainWindow(app::Application *app);

      private:
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
