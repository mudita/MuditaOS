// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

#include <application-settings/models/display-keypad/DarkModeModel.hpp>

#include <memory>

namespace gui
{
    class DisplayAndKeypadWindow : public OptionWindow
    {
      public:
        DisplayAndKeypadWindow(app::ApplicationCommon *app, std::unique_ptr<dark_mode::DarkModeModel> &&darkMode);

      private:
        void switchDisplayMode();

        std::list<Option> displayAndKeypadOptionsList();
        std::unique_ptr<dark_mode::DarkModeModel> darkMode;
    };
} // namespace gui
