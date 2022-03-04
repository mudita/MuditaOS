// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LayoutWindowPresenter.hpp"
#include <service-appmgr/Controller.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>

#include <EventStore.hpp>
#include <Temperature.hpp>
#include <service-appmgr/Constants.hpp>

constexpr auto alarmTime              = 1000 * 60 * 60 * 12;
constexpr auto clockTime              = 1000 * 60 * 60 * 12;
constexpr Store::Battery batteryState = {
    .levelState = Store::Battery::LevelState::Normal,
    .state      = Store::Battery::State::Discharging,
    .level      = 100,
};
constexpr utils::temperature::Temperature temperature = {
    .unit  = utils::temperature::Temperature::Unit::Celsius,
    .value = 21.0f,
};

namespace app::bell_settings
{
    LayoutWindowPresenter::LayoutWindowPresenter(app::ApplicationCommon *app,
                                                 std::unique_ptr<AbstractLayoutModel> &&layoutModel,
                                                 std::unique_ptr<AbstractTimeModel> &&timeModel)
        : app(app), layoutModel{std::move(layoutModel)}, timeModel{std::move(timeModel)}
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> LayoutWindowPresenter::getLayouts() const
    {
        std::vector<gui::Item *> layouts;

        for (auto const &option : layoutOptions) {
            layouts.push_back(option.first);
        }

        return layouts;
    }

    gui::Item *LayoutWindowPresenter::getSelectedLayout() const
    {
        const auto layoutSelected = layoutModel->getValue();

        for (auto const &option : layoutOptions) {
            if (option.second == layoutSelected) {
                return option.first;
            }
        }

        return layoutOptions.at(0).first;
    }

    void LayoutWindowPresenter::setLayout(gui::Item *selectedLayout)
    {
        for (auto const &option : layoutOptions) {
            if (option.first == selectedLayout) {
                layoutModel->setValue(option.second);
                auto layoutChangeRequest = std::make_shared<ChangeHomescreenLayoutMessage>(option.second);
                app->bus.sendUnicast(layoutChangeRequest, service::name::appmgr);
                break;
            }
        }
    }

    void LayoutWindowPresenter::initLayoutOptions()
    {
        auto layoutsList = gui::homeScreenLayouts();

        auto layoutClassicWithTemp = layoutsList.at("ClassicWithTemp")();
        layoutClassicWithTemp->setAlarmEdit(false);
        layoutClassicWithTemp->setAlarmActive(true);
        layoutClassicWithTemp->setTime(clockTime);
        // layoutClassicWithTemp->setTimeFormat(timeModel->getTimeFormat());
        layoutClassicWithTemp->setAlarmTime(alarmTime);
        layoutClassicWithTemp->setBatteryLevelState(batteryState);
        layoutClassicWithTemp->setViewState(app::home_screen::ViewState::Activated);
        layoutClassicWithTemp->setTemperature(temperature);

        auto layoutClassicWithBattery = layoutsList.at("ClassicWithBattery")();
        layoutClassicWithBattery->setAlarmEdit(false);
        layoutClassicWithBattery->setAlarmActive(true);
        layoutClassicWithBattery->setTime(clockTime);
        // Commented out to remove "AM"/"PM" from time spinner
        // layoutClassicWithBattery->setTimeFormat(timeModel->getTimeFormat());
        layoutClassicWithBattery->setAlarmTime(alarmTime);
        layoutClassicWithBattery->setBatteryLevelState(batteryState);
        layoutClassicWithBattery->setViewState(app::home_screen::ViewState::Activated);

        auto layoutClassicWithAmPm = layoutsList.at("ClassicWithAmPm")();
        layoutClassicWithAmPm->setAlarmEdit(false);
        layoutClassicWithAmPm->setAlarmActive(true);
        layoutClassicWithAmPm->setTime(clockTime);
        // layoutClassicWithAmPm->setTimeFormat(timeModel->getTimeFormat());
        layoutClassicWithAmPm->setAlarmTime(alarmTime);
        layoutClassicWithAmPm->setBatteryLevelState(batteryState);
        layoutClassicWithAmPm->setViewState(app::home_screen::ViewState::Activated);

        auto layoutClassic = layoutsList.at("Classic")();
        layoutClassic->setAlarmEdit(false);
        layoutClassic->setAlarmActive(true);
        layoutClassic->setTime(clockTime);
        // layoutClassic->setTimeFormat(timeModel->getTimeFormat());
        layoutClassic->setAlarmTime(alarmTime);
        layoutClassic->setBatteryLevelState(batteryState);
        layoutClassic->setViewState(app::home_screen::ViewState::Activated);

        layoutOptions.push_back({layoutClassicWithTemp->getLayout(), "ClassicWithTemp"});
        layoutOptions.push_back({layoutClassicWithBattery->getLayout(), "ClassicWithBattery"});
        layoutOptions.push_back({layoutClassicWithAmPm->getLayout(), "ClassicWithAmPm"});
        layoutOptions.push_back({layoutClassic->getLayout(), "Classic"});
    }
} // namespace app::bell_settings
