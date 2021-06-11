// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class BellMainWindow : public AppWindow
    {
      public:
        explicit BellMainWindow(app::Application *app);

        void buildInterface() override;
    };
} // namespace gui
