// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeypadLightWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <OptionSetting.hpp>

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <i18n/i18n.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app,
                                         app::settingsInterface::KeypdBacklightSettings *settings)
        : BaseSettingsWindow(app, window::name::keypad_light), keypadLightSettings(settings)
    {
        const auto keypadBacklightState = keypadLightSettings->getKeypadBacklightState();
        switch (keypadBacklightState) {
        case bsp::keypad_backlight::State::on:
            isAlwaysOnSwitchOn = true;
            break;
        case bsp::keypad_backlight::State::activeMode:
            isActiveSwitchOn = true;
            break;
        case bsp::keypad_backlight::State::off:
            isOffSwitchOn = true;
            break;
        }

        setTitle(utils::localize.get("app_settings_display_keypad_light"));
    }

    void KeypadLightWindow::switchHandler(bool &toggleSwitch)
    {
        isActiveSwitchOn   = false;
        isOffSwitchOn      = false;
        isAlwaysOnSwitchOn = false;
        toggleSwitch = !toggleSwitch;
        rebuildOptionList();
        if (isAlwaysOnSwitchOn) {
            keypadLightSettings->setKeypadBacklightState(bsp::keypad_backlight::State::on);
        }
        else if (isActiveSwitchOn) {
            keypadLightSettings->setKeypadBacklightState(bsp::keypad_backlight::State::activeMode);
        }
        else {
            keypadLightSettings->setKeypadBacklightState(bsp::keypad_backlight::State::off);
        }
    }

    auto KeypadLightWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        auto addCheckOption = [&](UTF8 text, bool &Switch) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text,
                [&](gui::Item &item) mutable {
                    switchHandler(Switch);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                Switch ? gui::option::SettingRightItem::Checked : gui::option::SettingRightItem::Disabled));
        };

        addCheckOption(utils::translateI18("app_settings_display_keypad_light_on"), isAlwaysOnSwitchOn);
        addCheckOption(utils::translateI18("app_settings_display_keypad_light_off"), isOffSwitchOn);
        addCheckOption(utils::translateI18("app_settings_display_keypad_light_active"), isActiveSwitchOn);

        return optionsList;
    } // namespace gui

} // namespace gui
