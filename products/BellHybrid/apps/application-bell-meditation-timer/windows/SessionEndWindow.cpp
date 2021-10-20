// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "SessionEndWindow.hpp"

#include <service-appmgr/Controller.hpp>

namespace gui
{
    SessionEndWindow::SessionEndWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::meditation::SessionEndedPresenterContract::Presenter> winPresenter)
        : BellFinishedWindow(app, gui::name::window::sessionEnded), presenter{std::move(winPresenter)}
    {
        timerCallback = [this](Item &, sys::Timer &) {
            presenter->activate();
            return true;
        };
    }

    bool SessionEndWindow::onInput(const InputEvent &inputEvent)
    {
        return true;
    }

    void SessionEndWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        BellFinishedWindow::onBeforeShow(mode, data);

        icon->image->set("big_namaste", ImageTypeSpecifier::W_G);
        icon->text->setRichText(utils::translate("app_meditation_bell_thank_you_for_session"));
    }
} // namespace gui
