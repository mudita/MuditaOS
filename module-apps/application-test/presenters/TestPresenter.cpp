// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TestPresenter.hpp"

namespace gui
{
    TestPresenter::TestPresenter(app::ApplicationTest *app) : app(app)
    {}

    void TestPresenter::attachWindow(TestMainWindow *window)
    {
        this->windowMain = window;
    }

    void TestPresenter::changeButtonState()
    {
        switch (state) {
        case ButtonState::Disabled:
            state = ButtonState::Enabled;
            break;
        case ButtonState::Enabled:
            state = ButtonState::Disabled;
            break;
        }
    }

    UTF8 TestPresenter::getTestButtonText()
    {
        if (state == ButtonState::Disabled) {
            return utils::translate("disabled");
        }
        return utils::translate("enabled");
    }

    void TestPresenter::handleTestButtonPressed()
    {
        changeButtonState();
        windowMain->changeButtonText(getTestButtonText());
    }
} // namespace gui
