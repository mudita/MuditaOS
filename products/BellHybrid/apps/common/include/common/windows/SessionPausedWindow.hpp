// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <gui/widgets/Icon.hpp>
#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

namespace gui
{
    namespace window::session_paused
    {
        inline constexpr auto sessionPaused = "SessionPausedWindow";
    } // namespace window::session_paused

    class SessionPausedWindow : public AppWindow
    {
      public:
        explicit SessionPausedWindow(app::ApplicationCommon *app);

        // virtual methods
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        Icon *icon{};
    };
} // namespace gui
