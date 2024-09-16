// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
        explicit UiTestWindow(app::ApplicationCommon *app);
    };
} // namespace gui
