// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/options/BellOptionWindow.hpp>

namespace app::meditation
{
    class MeditationMainWindow : public gui::BellOptionWindow
    {
      public:
        static constexpr auto defaultName = gui::name::window::main_window;
        explicit MeditationMainWindow(app::ApplicationCommon *app);

      private:
        std::list<gui::Option> settingsOptionsList();
    };

} // namespace app::meditation
