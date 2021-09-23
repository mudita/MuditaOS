// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>

#include "MeditationStyle.hpp"
#include "MeditationItem.hpp"

using namespace app::meditation;
using namespace app::meditationStyle;

namespace gui
{

    class MeditationWindow : public AppWindow
    {
      protected:
        bool inputDisabled;

        void gotoWindow(std::string name);
        void disableInput();

        virtual void buildMeditationItem(MeditationItem &item)       = 0;
        virtual void onMeditationItemAvailable(MeditationItem *item) = 0;

      public:
        MeditationWindow() = delete;
        MeditationWindow(app::ApplicationCommon *app, std::string name);

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} /* namespace gui */
