// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeypadLightWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::ApplicationCommon *app,
                                         app::settingsInterface::KeypdBacklightSettings *settings)
        : BaseSettingsWindow(app, window::name::keypad_light), keypadLightSettings(settings)
    {
        const auto keypadBacklightState = keypadLightSettings->getKeypadBacklightState();

        switch (keypadBacklightState) {
        case bsp::keypad_backlight::State::activeMode:
            isActiveSwitchOn    = true;
            break;
        case bsp::keypad_backlight::State::off:
            isActiveSwitchOn = false;
            break;
        }

        setTitle(utils::translate("app_settings_display_keypad_light"));
    }

    void KeypadLightWindow::switchHandler(bool &toggleSwitch)
    {
        toggleSwitch       = !toggleSwitch;
        if (isActiveSwitchOn) {
            keypadLightSettings->setKeypadBacklightState(bsp::keypad_backlight::State::activeMode);
        }
        else {
            keypadLightSettings->setKeypadBacklightState(bsp::keypad_backlight::State::off);
        }
        refreshOptionsList();
    }

    auto KeypadLightWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_keypad_light"),
            [&](gui::Item &item) mutable {
                switchHandler(isActiveSwitchOn);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                }
                return true;
            },
            this,
            isActiveSwitchOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));

        return optionsList;
    }
} // namespace gui
