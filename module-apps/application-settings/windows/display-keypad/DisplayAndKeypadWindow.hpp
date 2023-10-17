// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

#include <application-settings/models/display-keypad/DisplayModeModel.hpp>

#include <memory>

namespace gui
{
    class DisplayAndKeypadWindow : public OptionWindow
    {
      public:
        DisplayAndKeypadWindow(app::ApplicationCommon *app,
                               std::unique_ptr<display_mode::DisplayModeModel> &&displayMode);

      private:
        void switchDisplayMode();
        std::list<Option> displayAndKeypadOptionsList(bool isDarkModeEnabled);

        std::unique_ptr<display_mode::DisplayModeModel> displayMode;
    };
} // namespace gui
