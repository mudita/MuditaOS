// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>
#include <Utils.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <Text.hpp>

namespace gui
{
    namespace name::window
    {
        inline constexpr auto session_paused = "Sension paused";
    }

    class SessionPausedWindow : public AppWindow
    {
      public:
        explicit SessionPausedWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        gui::Image *appImage = nullptr;
        gui::Label *text     = nullptr;
    };
} // namespace gui
