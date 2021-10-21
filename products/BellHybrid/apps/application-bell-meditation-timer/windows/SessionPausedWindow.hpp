// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Icon.hpp>
#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

namespace gui
{
    class SessionPausedWindow : public AppWindow
    {
      public:
        SessionPausedWindow(app::ApplicationCommon *app);

        // virtual methods
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        Icon *icon{};
    };
} // namespace gui
