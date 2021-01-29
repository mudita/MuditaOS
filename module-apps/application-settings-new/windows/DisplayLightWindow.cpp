// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayLightWindow.hpp"
#include "application-settings-new/widgets/OptionSetting.hpp"
#include "OptionSetting.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include "GuiTimer.hpp"

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

        timerTask = std::make_unique<app::GuiTimer>(
            "AmbientLightTimer", application, gui::lighting::AMBIENT_LIGHT_TIMER_MS, Timer::Type::Continous);
        timerCallback = [this](Item &it, Timer &task) { return onTimerTimeout(it, task); };
        timerTask->start();
        application->connect(std::move(timerTask), this);
    }

    DisplayLightWindow::~DisplayLightWindow()
    {
        if (timerTask != nullptr) {
            timerTask->stop();
        }
    }

    auto DisplayLightWindow::onTimerTimeout(Item &self, Timer &task) -> bool
    {
        ambientLight = bsp::light_sensor::readout();
        refreshOptions();
        return true;
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

        auto addDisplayLight = [&](UTF8 text) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text, nullptr, nullptr, this, gui::option::SettingRightItem::Disabled));
        };

        addOnOffOoption(utils::translateI18("app_settings_display_light_main"), isDisplayLightSwitchOn);
        if (isDisplayLightSwitchOn) {
            addOnOffOoption(utils::translateI18("app_settings_display_light_auto"), isAutoLightSwitchOn);
        }

        if (isDisplayLightSwitchOn && !isAutoLightSwitchOn) {
            addBrightnessOption(optionsList);
        }

        addDisplayLight("Light intensity = " + utils::to_string(ambientLight));
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
