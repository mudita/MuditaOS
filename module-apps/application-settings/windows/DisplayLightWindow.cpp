// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"
#include "DisplayLightWindow.hpp"
#include "SpinBoxOptionSettings.hpp"

#include <i18n/i18n.hpp>
#include <OptionSetting.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app, app::settingsInterface::ScreenLightSettings *settings)
        : BaseSettingsWindow(app, window::name::display_light), screenLightSettings(settings)
    {
        auto values = screenLightSettings->getCurrentValues();

        isDisplayLightSwitchOn = values.lightOn;
        isAutoLightSwitchOn    = values.mode == screen_light_control::ScreenLightMode::Automatic;
        brightnessValue        = values.parameters.manualModeBrightness;

        setTitle(utils::translate("app_settings_display_display_light"));

        timerCallback = [this](Item &it, sys::Timer &task) { return onTimerTimeout(it, task); };
        timerTask     = app::GuiTimerFactory::createPeriodicTimer(
            application, this, "AmbientLightTimer", std::chrono::milliseconds{gui::lighting::AMBIENT_LIGHT_TIMER_MS});
        timerTask.start();
    }

    DisplayLightWindow::~DisplayLightWindow()
    {
        if (timerTask.isActive()) {
            timerTask.stop();
        }
    }

    auto DisplayLightWindow::onTimerTimeout(Item &self, sys::Timer &task) -> bool
    {
        ambientLight    = bsp::light_sensor::readout();
        auto values     = screenLightSettings->getCurrentValues();
        brightnessValue = values.parameters.manualModeBrightness;
        refreshOptionsList();

        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        return true;
    }

    void DisplayLightWindow::switchHandler(bool &onOffSwitch)
    {
        onOffSwitch = !onOffSwitch;
        screenLightSettings->setStatus(isDisplayLightSwitchOn);
        screenLightSettings->setMode(isAutoLightSwitchOn);

        refreshOptionsList();
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
                        this->setBottomBarText(utils::translate(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                toggle ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
        };

        auto addDisplayLight = [&](UTF8 text) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text, nullptr, nullptr, this, gui::option::SettingRightItem::Disabled));
        };

        auto addBrightnessValue = [&](UTF8 text) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text, nullptr, nullptr, this, gui::option::SettingRightItem::Disabled));
        };

        addOnOffOoption(utils::translate("app_settings_display_light_main"), isDisplayLightSwitchOn);
        if (isDisplayLightSwitchOn) {
            addOnOffOoption(utils::translate("app_settings_display_light_auto"), isAutoLightSwitchOn);
        }

        if (isDisplayLightSwitchOn && !isAutoLightSwitchOn) {
            addBrightnessOption(optionsList);
        }

        addDisplayLight("Light intensity = " + utils::to_string(ambientLight));
        addBrightnessValue("Manual brightness = " + utils::to_string(brightnessValue) + "%");

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
                setBottomBarText(utils::translate(style::strings::common::set), BottomBar::Side::CENTER);
            }
            return true;
        };

        auto spinner = std::make_unique<gui::SpinBoxOptionSettings>(
            utils::translate("app_settings_display_light_brightness"),
            std::ceil(brightnessValue / brightnessStep),
            std::ceil(screen_light_control::ManualModeParameters::MAX_BRIGHTNESS / brightnessStep),
            setBrightness,
            setBottomBarOnSpinnerFocus);

        return spinner;
    }

    void DisplayLightWindow::addBrightnessOption(std::list<gui::Option> &options)
    {
        options.emplace_back(createBrightnessOption(lighting::LIGHT_CONTROL_STEP));
    }
} // namespace gui
