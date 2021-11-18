// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class ClosingWindow : public AppWindow
    {
      public:
        explicit ClosingWindow(app::ApplicationCommon *app);
        void buildInterface() override;
    };
} /* namespace gui */
