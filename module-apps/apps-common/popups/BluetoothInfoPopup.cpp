// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothInfoPopup.hpp"
#include "data/PopupRequestParams.hpp"

#include <service-appmgr/Controller.hpp>
#include <ApplicationCommon.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

BluetoothInfoPopup::BluetoothInfoPopup(app::ApplicationCommon *app, const std::string &name)
    : WindowWithTimer(app, name)
{
    buildInterface();
}

void BluetoothInfoPopup::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("bluetooth_popup"));

    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::ok));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "",
                             utils::translate(""));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}

void BluetoothInfoPopup::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto infoData = dynamic_cast<BluetoothInfoParams *>(data);

    if (infoData == nullptr) {
        LOG_ERROR("No info data received");
        return;
    }

    if (infoData->isSucceed()) {
        infoIcon->image->set("success_128px_W_G");
        infoIcon->text->setRichText(utils::translate("bluetooth_info_popup_success"),
                                    {{"$DEVICE", std::string(infoData->getDevice().name.data())}});

        timerCallback = [this](Item &, sys::Timer &timer) {
            closePopups();
            return true;
        };
        resetTimer();
    }
    else {
        infoIcon->image->set("fail_128px_W_G");
        infoIcon->text->setRichText(utils::translate("bluetooth_info_popup_error"),
                                    {{"$DEVICE", std::string(infoData->getDevice().name.data())},
                                     {"$ERROR", std::string(infoData->getErrorCode())}});
    }
}

bool BluetoothInfoPopup::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
        closePopups();
        return true;
    }
    return false;
}

void BluetoothInfoPopup::closePopups()
{
    app::manager::Controller::sendAction(
        application,
        app::manager::actions::AbortPopup,
        std::make_unique<gui::PopupRequestParams>(gui::popup::ID::BluetoothAuthenticate));
    app::manager::Controller::sendAction(application,
                                         app::manager::actions::AbortPopup,
                                         std::make_unique<gui::PopupRequestParams>(gui::popup::ID::BluetoothInfo));
}
