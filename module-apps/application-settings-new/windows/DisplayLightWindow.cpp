// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayLightWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "windows/OptionSetting.hpp"

#include <i18/i18.hpp>

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app) : BaseSettingsWindow(app, window::name::display_light)
    {
        setTitle(utils::localize.get("app_settings_display_display_light"));
    }

    void DisplayLightWindow::switchHandler(bool &onOffSwitch)
    {
        onOffSwitch = !onOffSwitch;
        rebuildOptionList();
    }

    auto DisplayLightWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        auto addOnOffOoption = [&](UTF8 text, bool &toggle) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                text,
                [&](gui::Item &item) mutable {
                    switchHandler(toggle);
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
                toggle ? RightItem::On : RightItem::Off));
        };

        addOnOffOoption(utils::translateI18("app_settings_display_light_main"), isDisplayLightSwitchOn);
        if (isDisplayLightSwitchOn) {
            addOnOffOoption(utils::translateI18("app_settings_display_light_auto"), isAutoLightSwitchOn);
        }

        if (isDisplayLightSwitchOn && !isAutoLightSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_display_light_brightness"),
                [=](gui::Item &item) { return true; },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::set),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                RightItem::ArrowWhite));
        }

        return optionsList;
    }

} // namespace gui
