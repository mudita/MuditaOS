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

top_bar::Configuration PhoneLockChangeInfoWindow::configureTopBar(top_bar::Configuration appConfiguration)
{
    appConfiguration.enable(top_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.enable(top_bar::Indicator::Time);
    appConfiguration.enable(top_bar::Indicator::PhoneMode);
    appConfiguration.enable(top_bar::Indicator::Battery);
    appConfiguration.enable(top_bar::Indicator::Signal);
    appConfiguration.enable(top_bar::Indicator::SimCard);
    return appConfiguration;
}

void PhoneLockChangeInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    WindowWithTimer::onBeforeShow(mode, data);

    if (auto infoData = dynamic_cast<locks::LockData *>(data)) {

        switch (infoData->getPhoneLockInputTypeAction()) {
        case locks::PhoneLockInputTypeAction::Disable:
            infoText->setRichText(utils::translate("phone_lock_disabled"));
            break;
        case locks::PhoneLockInputTypeAction::Enable:
        case locks::PhoneLockInputTypeAction::Change:
            infoText->setRichText(utils::translate("phone_lock_changed_successfully"));
            break;
        default:
            break;
        }
    }
}

void PhoneLockChangeInfoWindow::buildInterface()
{
    namespace lock_style = style::window::pin_lock;
    AppWindow::buildInterface();

    setTitle(utils::translate("phone_lock_configure"));

    infoImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, "success_icon_W_G");
    infoText  = new Text(this,
                        lock_style::primary_text::x,
                        lock_style::primary_text::y,
                        lock_style::primary_text::w,
                        lock_style::primary_text::h);

    infoText->setAlignment(Alignment::Horizontal::Center);
}
