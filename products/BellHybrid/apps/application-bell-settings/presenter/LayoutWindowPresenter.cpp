// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "LayoutWindowPresenter.hpp"
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>

#include <EventStore.hpp>
#include <Temperature.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>

namespace
{
    constexpr auto alarmTime = 0;
    constexpr auto clockTime = 0;

    constexpr Store::Battery batteryState = {
        .levelState = Store::Battery::LevelState::Normal,
        .state      = Store::Battery::State::Discharging,
        .level      = 100,
    };

    constexpr utils::temperature::Temperature temperature = {
        .unit  = utils::temperature::Temperature::Unit::Celsius,
        .value = 21.0f,
    };
} // namespace

namespace app::bell_settings
{
    LayoutWindowPresenter::LayoutWindowPresenter(app::ApplicationCommon *app,
                                                 std::unique_ptr<AbstractLayoutModel> &&layoutModel,
                                                 std::unique_ptr<AbstractTimeModel> &&timeModel,
                                                 std::unique_ptr<AbstractQuoteModel> &&quoteModel)
        : app(app), layoutModel{std::move(layoutModel)}, timeModel{std::move(timeModel)}, quoteModel{
                                                                                              std::move(quoteModel)}
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> LayoutWindowPresenter::getLayouts() const
    {
        std::vector<gui::Item *> layouts;
        layouts.reserve(layoutOptions.size());

        for (const auto &[option, _] : layoutOptions) {
            layouts.push_back(option->getLayout());
        }

        return layouts;
    }

    gui::Item *LayoutWindowPresenter::getSelectedLayout() const
    {
        const auto &layoutSelected = layoutModel->getValue();

        for (const auto &[option, name] : layoutOptions) {
            if (name == layoutSelected) {
                return option->getLayout();
            }
        }

        return layoutOptions.at(0).first->getLayout();
    }

    void LayoutWindowPresenter::setLayout(gui::Item *selectedLayout)
    {
        for (const auto &[option, name] : layoutOptions) {
            if (option->getLayout() == selectedLayout) {
                layoutModel->setValue(name);
                auto layoutChangeRequest = std::make_shared<ChangeHomescreenLayoutMessage>(name);
                app->bus.sendUnicast(std::move(layoutChangeRequest), service::name::appmgr);
                break;
            }
        }
    }

    void LayoutWindowPresenter::initLayoutOptions()
    {
        quoteModel->setCallback([=](const auto &quote, const auto &author) {
            for (auto &[layout, _] : layoutOptions) {
                layout->setQuoteText(quote, author);
            }
        });
        quoteModel->sendQuery();
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
            layoutOptions.emplace_back(layout, layoutEntry.first);
        }
    }
} // namespace app::bell_settings
