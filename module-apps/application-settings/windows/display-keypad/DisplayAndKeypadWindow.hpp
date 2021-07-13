// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    class DisplayAndKeypadWindow : public OptionWindow
    {
      public:
        explicit DisplayAndKeypadWindow(app::Application *app);

      private:
        std::list<Option> displayAndKeypadOptionsList();
    };
} // namespace gui
