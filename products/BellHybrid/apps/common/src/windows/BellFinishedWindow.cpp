// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellFinishedWindow.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <common/data/StyleCommon.hpp>
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
        switch (exitBehaviour) {
        case BellFinishedWindowData::ExitBehaviour::CloseApplication:
            app::manager::Controller::switchBack(application);
            break;
        case BellFinishedWindowData::ExitBehaviour::SwitchWindow:
            application->switchWindow(windowToReturn);
            break;
        case BellFinishedWindowData::ExitBehaviour::ReturnToHomescreen:
            app::manager::Controller::sendAction(application, app::manager::actions::Home);
            break;
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
            icon->text->setVisible(false);
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
            if (not metadata->text.empty()) {
                icon->text->setRichText(metadata->text);
                icon->text->setVisible(true);
            }
            icon->resizeItems();
            windowToReturn = metadata->windowToReturn;
            exitBehaviour  = metadata->exitBehaviour;
            if (metadata->timeout != std::chrono::seconds::zero()) {
                resetTimer(metadata->timeout);
            }
        }
    }

} // namespace gui
