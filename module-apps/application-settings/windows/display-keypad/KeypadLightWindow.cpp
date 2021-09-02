// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeypadLightWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app,
                                         app::settingsInterface::KeypdBacklightSettings *settings)
        : BaseSettingsWindow(app, window::name::keypad_light), keypadLightSettings(settings)
    {
        const auto keypadBacklightState = keypadLightSettings->getKeypadBacklightState();

        switch (keypadBacklightState) {
        case bsp::keypad_backlight::State::on:
            isAlwaysOnSwitchOn  = true;
            selectedOptionIndex = 0;
            break;
        case bsp::keypad_backlight::State::activeMode:
            isActiveSwitchOn    = true;
            selectedOptionIndex = 1;
            break;
        case bsp::keypad_backlight::State::off:
            isOffSwitchOn       = true;
            selectedOptionIndex = 2;
            break;
        }

        setTitle(utils::translate("app_settings_display_keypad_light"));
    }

    void KeypadLightWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        refreshOptionsList(selectedOptionIndex);
    }

    void KeypadLightWindow::switchHandler(bool &toggleSwitch)
    {
        isActiveSwitchOn   = false;
        isOffSwitchOn      = false;
        isAlwaysOnSwitchOn = false;
        toggleSwitch       = !toggleSwitch;
        refreshOptionsList();
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
                        this->setBottomBarText(utils::translate(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                Switch ? gui::option::SettingRightItem::Checked : gui::option::SettingRightItem::Disabled));
        };

        addCheckOption(utils::translate("app_settings_display_keypad_light_on"), isAlwaysOnSwitchOn);
        addCheckOption(utils::translate("app_settings_display_keypad_light_active"), isActiveSwitchOn);
        addCheckOption(utils::translate("app_settings_display_keypad_light_off"), isOffSwitchOn);

        return optionsList;
    }
} // namespace gui
