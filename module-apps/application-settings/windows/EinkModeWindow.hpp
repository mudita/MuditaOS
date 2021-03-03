// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui
{
    namespace window::name
    {
        inline constexpr auto eink = "EinkModeWindow";
    } // namespace window::name

    class EinkModeWindow : public AppWindow
    {
      public:
        explicit EinkModeWindow(app::Application *app);
    };
} /* namespace gui */
