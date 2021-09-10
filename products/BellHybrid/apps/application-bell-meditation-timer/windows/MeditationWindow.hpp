// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>

#include "../data/MeditationStyle.hpp"
#include "../data/MeditationItem.hpp"

using namespace meditation;
using namespace meditationStyle;

namespace gui
{

    class MeditationWindow : public AppWindow
    {
      protected:
        MeditationItem item;
        bool inputDisabled;

        void gotoWindow(std::string name);
        void disableInput();

      public:
        MeditationWindow() = delete;
        explicit MeditationWindow(app::Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} /* namespace gui */
