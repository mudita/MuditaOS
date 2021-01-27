// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeypadLightWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app,
                                         app::settingsInterface::KeypdBacklightSettings *settings)
        : BaseSettingsWindow(app, window::name::keypad_light), keypadLightSettings(settings)
    {
        if (keypadLightSettings->isKeypadBacklightOn()) {
            isAlwaysOnSwitchOn = true;
        }
        else {
            isOffSwitchOn = true;
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
        keypadLightSettings->setKeypadBacklightState(isAlwaysOnSwitchOn);
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
