// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNameWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "widgets/InputBox.hpp"

#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/SetDeviceName.hpp>

#include <Utils.hpp>
#include <application-settings-new/data/PhoneNameData.hpp>

namespace gui
{
    PhoneNameWindow::PhoneNameWindow(app::Application *app) : AppWindow(app, gui::window::name::phone_name)
    {
        buildInterface();
    }

    void PhoneNameWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_settings_bluetooth_phone_name"));

        inputField = inputBox(this, utils::localize.get("app_settings_bluetooth_phone_name"));
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestDeviceName>(),
                                     service::name::bluetooth);

        setFocusItem(inputField);
    }

    void PhoneNameWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        inputField->clear();
        if (const auto newData = dynamic_cast<PhoneNameData *>(data); data != nullptr) {
            inputField->setText(newData->getName());
        }
    }

    auto PhoneNameWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_ENTER) && !inputField->isEmpty()) {
            auto result = std::make_shared<::message::bluetooth::SetDeviceName>(inputField->getText());
            application->bus.sendUnicast(std::move(result), service::name::bluetooth);
            return true;
        }

        return false;
    }
} // namespace gui
