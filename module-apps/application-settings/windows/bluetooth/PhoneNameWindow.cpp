// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNameWindow.hpp"

#include <application-settings/data/PhoneNameData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <widgets/InputBox.hpp>

namespace gui
{
    PhoneNameWindow::PhoneNameWindow(app::ApplicationCommon *app,
                                     std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel)
        : AppWindow(app, gui::window::name::phone_name), bluetoothSettingsModel(bluetoothSettingsModel)
    {
        bluetoothSettingsModel->requestDeviceName();
        buildInterface();
    }

    void PhoneNameWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_bluetooth_phone_name"));

        inputField = inputBox(this, utils::translate("app_settings_bluetooth_phone_name"));
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setFocusItem(inputField);
    }

    void PhoneNameWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        erase(inputField);
        inputField = nullptr;
    }

    void PhoneNameWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        if (const auto newData = dynamic_cast<PhoneNameData *>(data); data != nullptr) {
            inputField->setText(newData->getName());
            inputField->setTextLimitType(gui::TextLimitType::MaxSignsCount, maxNameLength);
        }
    }

    auto PhoneNameWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER) && !inputField->isEmpty()) {
            bluetoothSettingsModel->setDeviceName(inputField->getText());
            application->returnToPreviousWindow();
            return true;
        }

        return false;
    }
} // namespace gui
