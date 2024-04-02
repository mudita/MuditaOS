// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FocusCommon.hpp"
#include <common/options/BellOptionWindow.hpp>

namespace app::focus
{
    class FocusMainWindow : public gui::BellOptionWindow
    {
      public:
        explicit FocusMainWindow(app::ApplicationCommon *app, const std::string name = window::name::main);

      private:
        std::list<gui::Option> settingsOptionsList();
    };

} // namespace app::focus
