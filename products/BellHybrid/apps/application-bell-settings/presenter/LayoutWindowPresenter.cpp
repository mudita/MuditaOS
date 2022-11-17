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

constexpr auto alarmTime              = 0;
constexpr auto clockTime              = 0;
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
        const auto timeFormat = timeModel->getTimeFormat();
        auto layoutsList      = timeFormat == utils::time::Locale::TimeFormat::FormatTime24H
                                    ? gui::factory::getLayoutsFormat24h()
                                    : gui::factory::getLayoutsFormat12h();

        for (auto &layoutEntry : layoutsList) {
            auto layout = layoutEntry.second();
            layout->setViewState(app::home_screen::ViewState::Activated);
            layout->setTimeFormat(timeFormat);
            layout->setTime(clockTime);
            layout->setAlarmTimeFormat(timeFormat);
            layout->setAlarmTime(alarmTime);
            layout->setBatteryLevelState(batteryState);
            layout->setTemperature(temperature);
            layoutOptions.push_back({layout->getLayout(), layoutEntry.first});
        }
    }
} // namespace app::bell_settings
