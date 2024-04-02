// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusTimerWindow.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace app::focus
{
    FocusTimerWindow::FocusTimerWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<FocusTimerContract::Presenter> &&windowPresenter,
                                       const std::string &name)
        : AppWindow(app, name), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void FocusTimerWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }

    bool FocusTimerWindow::onInput(const gui::InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::focus
