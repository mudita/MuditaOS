// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>
#include <InputEvent.hpp>
#include <ListView.hpp>
#include <Utils.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

#include "MeditationWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto meditation_timer = "Meditation timer";
    }

    class MeditationTimerWindow : public MeditationWindow
    {
      public:
        explicit MeditationTimerWindow(app::ApplicationCommon *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        gui::Label *title  = nullptr;
        gui::Label *text   = nullptr;
        gui::Label *minute = nullptr;

        void increaseTimer();
        void decreaseTimer();
        void updateDisplay();
    };
} // namespace gui
