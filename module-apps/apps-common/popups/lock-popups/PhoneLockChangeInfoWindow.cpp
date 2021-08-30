// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockChangeInfoWindow.hpp"
#include <locks/data/LockStyle.hpp>
#include <locks/data/LockData.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

PhoneLockChangeInfoWindow::PhoneLockChangeInfoWindow(app::Application *app, const std::string &name)
    : WindowWithTimer(app, name)
{
    buildInterface();
}

void PhoneLockChangeInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    WindowWithTimer::onBeforeShow(mode, data);

    if (auto infoData = dynamic_cast<locks::LockData *>(data)) {

        switch (infoData->getPhoneLockInputTypeAction()) {
        case locks::PhoneLockInputTypeAction::Disable:
            infoIcon->text->setRichText(utils::translate("phone_lock_disabled"));
            break;
        case locks::PhoneLockInputTypeAction::Enable:
        case locks::PhoneLockInputTypeAction::Change:
            infoIcon->text->setRichText(utils::translate("phone_lock_changed_successfully"));
            break;
        default:
            break;
        }
    }
}

void PhoneLockChangeInfoWindow::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("phone_lock_configure"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "success_icon_W_G",
                             "");
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
