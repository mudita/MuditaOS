// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
