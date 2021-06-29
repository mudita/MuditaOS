// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/BaseSettingsWindow.hpp>
#include <Text.hpp>

namespace gui
{
    class TorchWindow : public BaseSettingsWindow
    {
      public:
        TorchWindow(app::Application *app);
        void buildInterface() override;

      private:
        Rect *bar             = nullptr;
        Text *descriptionText = nullptr;
        bool switchState      = false;
        auto buildOptionsList() -> std::list<Option> override;
        void toggleSwitchState();
    };
}; // namespace gui
