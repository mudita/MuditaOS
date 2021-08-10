// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>
#include <data/BellAlarmStyle.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BellAlarmWindow::BellAlarmWindow(
        app::Application *app,
        std::unique_ptr<app::bell_alarm::BellAlarmWindowContract::Presenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellAlarmWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        bottomBar->setVisible(false);

        body              = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height);
        timeSetFmtSpinner = new TimeSetFmtSpinner(body->centerBox);
        timeSetFmtSpinner->setFont(bell_alarm_style::time_set_fmt_spinner::font);

        setFocusItem(body);
    }

    bool BellAlarmWindow::onInput(const InputEvent &inputEvent)
    {
        if (body->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->saveData();
            application->returnToPreviousWindow();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellAlarmWindow::rebuild()
    {
        erase();
        buildInterface();
    }
} /* namespace gui */
