// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoEvents.hpp"
#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include <log.hpp>

#include <service-appmgr/Controller.hpp>
#include <header/AddElementAction.hpp>
#include <Image.hpp>

using namespace gui;

NoEvents::NoEvents(app::Application *app, const std::string &name) : gui::Dialog(app, name)
{
    header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);
}

void NoEvents::onBeforeShow(ShowMode mode, SwitchData *data)
{
    Dialog::onBeforeShow(mode, data);
    auto metadata = dynamic_cast<DialogMetadataMessage *>(data);
    if (metadata != nullptr) {
        auto foo      = metadata->get().action;
        inputCallback = [foo](Item &, const InputEvent &inputEvent) -> bool {
            if (foo && inputEvent.isShortRelease(KeyCode::KEY_LEFT)) {
                return foo();
            }
            return false;
        };
    }

    if (getTitle() == utils::translate("app_calendar_title_main")) {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate("app_calendar_bar_month"));
    }
}

bool NoEvents::onInput(const gui::InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) && getTitle() == utils::translate("app_calendar_title_main")) {
        app::manager::Controller::switchBack(application);
        return true;
    }

    if (AppWindow::onInput(inputEvent)) {
        return true;
    }

    if (inputEvent.isShortRelease(gui::KeyCode::KEY_LF) && getTitle() == utils::translate("app_calendar_title_main")) {
        application->switchWindow(gui::name::window::main_window);
        return true;
    }

    return false;
}
