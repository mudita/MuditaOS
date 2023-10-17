// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-test/ApplicationTest.hpp"
#include "utf8/UTF8.hpp"
#include "windows/TestWindow.hpp"

namespace gui
{

    class TestPresenter final
    {
        enum class ButtonState
        {
            Disabled,
            Enabled
        } state = ButtonState::Disabled;
        app::ApplicationTest *app;
        TestMainWindow *windowMain = nullptr;
        void changeButtonState();

      public:
        explicit TestPresenter(app::ApplicationTest *app);
        void attachWindow(TestMainWindow *window);

        void handleTestButtonPressed();
        UTF8 getTestButtonText();
    };
} // namespace gui
