// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class SystemMainWindow : public BaseSettingsWindow
    {
      public:
        explicit SystemMainWindow(app::ApplicationCommon *app);

      private:
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
