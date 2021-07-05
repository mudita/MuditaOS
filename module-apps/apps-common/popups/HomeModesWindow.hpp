// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/WindowWithTimer.hpp"

namespace gui
{
    class ModesBox;
    class HomeModesWindow : public WindowWithTimer
    {
      protected:
        ModesBox *modesBox = nullptr;

      public:
        HomeModesWindow(app::Application *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace gui
