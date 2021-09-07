// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Text.hpp>

namespace gui
{
    class UiTestWindow : public AppWindow
    {
      protected:
        gui::Text *text = nullptr;

      public:
        explicit UiTestWindow(app::Application *app);
    };
} // namespace gui
