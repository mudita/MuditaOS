// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/Application.hpp"
#include "AppWindow.hpp"
#include "module-apps/widgets/BarGraph.hpp"
#include "module-apps/widgets/ModesBox.hpp"
#include <functional>

namespace gui
{
    class HomeModesWindow : public AppWindow
    {
      protected:
        ModesBox *modesBox = nullptr;

      public:
        HomeModesWindow(app::Application *app, const std::string &name);

        ~HomeModesWindow() override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;

        void buildInterface() override;

        void destroyInterface() override;
    };
}; // namespace gui
