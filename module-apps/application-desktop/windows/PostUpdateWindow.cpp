// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PostUpdateWindow.hpp"

#include <locks/data/LockStyle.hpp>
#include <application-desktop/data/DesktopData.hpp>
#include <application-desktop/windows/Names.hpp>

#include <i18n/i18n.hpp>

using namespace gui;

PostUpdateWindow::PostUpdateWindow(app::Application *app)
    : AppWindow(app, app::window::name::desktop_post_update_window)
{
    buildInterface();
}

void PostUpdateWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (data == nullptr) {
        LOG_ERROR("Received null pointer");
    }
    else {
        auto *item = dynamic_cast<CurrentOsVersion *>(data);
        if (item != nullptr) {
            currentOsVersion = item->getCurrentOsVersion();
            auto info        = utils::translate("app_desktop_update_success");
            utils::findAndReplaceAll(info, "$VERSION", currentOsVersion);
            infoIcon->text->setText(info);
        }
    }
    setVisibleState();
}

void PostUpdateWindow::setVisibleState()
{
    bottomBar->setActive(BottomBar::Side::CENTER, true);
}

bool PostUpdateWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
        application->switchWindow(gui::name::window::main_window);
        return true;
    }
    return AppWindow::onInput(inputEvent);
}

void PostUpdateWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

status_bar::Configuration PostUpdateWindow::configureStatusBar(status_bar::Configuration appConfiguration)
{
    appConfiguration.enable(status_bar::Indicator::Time);
    appConfiguration.disable(status_bar::Indicator::Lock);
    appConfiguration.disable(status_bar::Indicator::Battery);
    appConfiguration.disable(status_bar::Indicator::Signal);
    appConfiguration.disable(status_bar::Indicator::SimCard);
    return appConfiguration;
}

void PostUpdateWindow::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("app_desktop_update_muditaos"));

    bottomBar->setText(BottomBar::Side::CENTER, utils::translate("common_ok"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "circle_success",
                             "");
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}

void PostUpdateWindow::destroyInterface()
{
    erase();
}
