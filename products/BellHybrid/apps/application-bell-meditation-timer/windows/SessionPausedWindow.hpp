// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <InputEvent.hpp>
#include "IconTextWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto session_paused = "Sension paused";
    }

    class SessionPausedWindow : public IconTextWindow
    {
      public:
        explicit SessionPausedWindow(app::Application *app);

        // virtual methods
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onTimeout() override;
        std::string getText() override;
        std::string getImageName() override;
        std::chrono::seconds getTimeout() const override;
    };
} // namespace gui
