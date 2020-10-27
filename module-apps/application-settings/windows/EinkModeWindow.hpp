// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui
{
    namespace window::name
    {
        inline const std::string eink = "EinkModeWindow";
    } // namespace name

    class EinkModeWindow : public AppWindow
    {
      public:
        EinkModeWindow(app::Application *app);
    };
} /* namespace gui */
