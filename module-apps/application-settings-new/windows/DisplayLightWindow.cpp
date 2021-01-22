// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayLightWindow.hpp"
#include "application-settings-new/widgets/OptionSetting.hpp"
#include "OptionSetting.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"

#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <i18n/i18n.hpp>
#include <Service/Bus.hpp>

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app, app::settingsInterface::ScreenLightSettings *settings)
        : BaseSettingsWindow(app, window::name::display_light), screenLightSettings(settings)
    {
        auto values = screenLightSettings->getCurrentValues();

        isDisplayLightSwitchOn = values.lightOn;
        isAutoLightSwitchOn    = values.mode == screen_light_control::ScreenLightMode::Automatic;
        brightnessValue        = values.parameters.manualModeBrightness;

        setTitle(utils::localize.get("app_settings_display_display_light"));
    }

    void DisplayLightWindow::switchHandler(bool &onOffSwitch)
    {
        onOffSwitch = !onOffSwitch;

        rebuildOptionList();

        screenLightSettings->setStatus(isDisplayLightSwitchOn);
        screenLightSettings->setMode(isAutoLightSwitchOn);
    }

    auto DisplayLightWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        auto addOnOffOoption = [&](UTF8 text, bool &toggle) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
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
                toggle ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
        };

        addOnOffOoption(utils::translateI18("app_settings_display_light_main"), isDisplayLightSwitchOn);
        if (isDisplayLightSwitchOn) {
            addOnOffOoption(utils::translateI18("app_settings_display_light_auto"), isAutoLightSwitchOn);
        }

        if (isDisplayLightSwitchOn && !isAutoLightSwitchOn) {
            addBrightnessOption(optionsList);
        }

        return optionsList;
    }

    auto DisplayLightWindow::createBrightnessOption(int brightnessStep) -> std::unique_ptr<SpinBoxOptionSettings>
    {
        auto setBrightness = [this, brightnessStep](uint8_t value) {
            screenLightSettings->setBrightness(value * brightnessStep);
            return true;
        };

        auto setBottomBarOnSpinnerFocus = [&](gui::Item &item) {
            if (item.focus) {
                setBottomBarText(utils::translateI18(style::strings::common::set), BottomBar::Side::CENTER);
            }
            return true;
        };

        auto spinner = std::make_unique<gui::SpinBoxOptionSettings>(
            utils::translateI18("app_settings_display_light_brightness") + " " + utils::to_string(brightnessStep),
            brightnessValue * brightnessStep,
            std::ceil(screen_light_control::Parameters::MAX_BRIGHTNESS / brightnessStep),
            setBrightness,
            setBottomBarOnSpinnerFocus);

        return spinner;
    }

    void DisplayLightWindow::addBrightnessOption(std::list<gui::Option> &options)
    {
        /*
         * We are adding 4 brightness widgets to easily check what is the best step for setting screen brightness.
         */
        for (auto step : {10, 15, 20, 25}) {
            options.emplace_back(createBrightnessOption(step));
        }
    }
} // namespace gui
