// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/BellFinishedWindow.hpp"
#include <apps-common/Application.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>

namespace gui
{

    BellFinishedWindow::BellFinishedWindow(app::Application *app) : WindowWithTimer(app, name)
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            application->switchWindow(windowToReturn);
            return true;
        };
    }
    void BellFinishedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
    }
    bool BellFinishedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(windowToReturn);
            return true;
        }
        return false;
    }
    void BellFinishedWindow::rebuild()
    {
        erase();
        buildInterface();
    }
    void BellFinishedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);

        if (auto metadata = dynamic_cast<Data *>(data)) {
            icon->image->set(metadata->icon);
            icon->text->setText(metadata->text);
            windowToReturn = metadata->windowToReturn;
        }
    }

} // namespace gui
