// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellFinishedWindow.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include "service-appmgr/Controller.hpp"

namespace gui
{

    BellFinishedWindow::BellFinishedWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer(app, name)
    {
        buildInterface();
        timerCallback = [this](Item &, sys::Timer &) {
            exit();
            return true;
        };
    }

    void BellFinishedWindow::exit()
    {
        if (closeApplication) {
            app::manager::Controller::switchBack(application);
        }
        else {
            application->switchWindow(windowToReturn);
        }
    }

    void BellFinishedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);
        if (icon == nullptr) {
            icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
            icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            icon->text->setFont(style::window::font::verybiglight);
        }
    }

    bool BellFinishedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            exit();
            return true;
        }
        return false;
    }

    void BellFinishedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);

        if (auto metadata = dynamic_cast<BellFinishedWindowData *>(data)) {
            icon->image->set(metadata->icon);
            icon->text->setRichText(metadata->text);
            icon->resizeItems();
            windowToReturn   = metadata->windowToReturn;
            closeApplication = metadata->closeApplication;
        }
    }

} // namespace gui
