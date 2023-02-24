// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"
#include "MeditationOptionsWindows.hpp"
#include "Names.hpp"

#include <Constants.hpp>
#include <OptionSetting.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

namespace
{
    std::string toString(std::chrono::seconds duration)
    {
        if (duration.count() >= 60) {
            return std::to_string(static_cast<int>((duration.count()) / 60)) + " m";
        }
        return std::to_string(static_cast<int>(duration.count())) + " s";
    }
} // namespace

MeditationOptionsWindow::MeditationOptionsWindow(app::ApplicationCommon *app)
    : OptionWindow(app, app::window::name::meditation_options)
{
    setTitle(utils::translate("common_options_title"));
    addOptions(buildOptionsList());
}

std::list<Option> MeditationOptionsWindow::buildOptionsList()
{
    std::list<Option> options;
    addCounterOption(options);
    addPreparationTimeOption(options);
    return options;
}

void MeditationOptionsWindow::addCounterOption(std::list<Option> &options)
{
    auto app = static_cast<app::ApplicationMeditation *>(application);
    options.emplace_back(std::make_unique<option::OptionSettings>(
        utils::translate("app_meditation_option_show_counter"),
        [=](Item &item) {
            app->state->setCounterVisible(!app->state->isCounterVisible());
            refreshOptions(buildOptionsList());
            return true;
        },
        [=](gui::Item &item) {
            if (item.focus) {
                setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
            }
            return true;
        },
        nullptr,
        app->state->isCounterVisible() ? option::SettingRightItem::On : option::SettingRightItem::Off));
}

void MeditationOptionsWindow::addPreparationTimeOption(std::list<Option> &options)
{
    options.emplace_back(std::make_unique<option::OptionSettings>(
        utils::translate("app_meditation_preparation_time"),
        [=](Item &item) {
            application->switchWindow(app::window::name::meditation_preparation);
            return true;
        },
        [=](gui::Item &item) {
            if (item.focus) {
                setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
            }
            return true;
        },
        nullptr,
        option::SettingRightItem::ArrowBlack));
}

PreparationTimeWindow::PreparationTimeWindow(app::ApplicationCommon *app)
    : OptionWindow(app, app::window::name::meditation_preparation)
{
    setTitle(utils::translate("app_meditation_preparation_time"));
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
    addOptions(buildOptionsList());
}

std::list<Option> PreparationTimeWindow::buildOptionsList()
{
    auto app = static_cast<app::ApplicationMeditation *>(application);
    std::list<Option> options;
    for (auto &&duration : Constants::Params::preparationTimes) {
        addPreparationTimeOption(duration, app, options);
    }
    return options;
}

void PreparationTimeWindow::addPreparationTimeOption(std::chrono::seconds duration,
                                                     app::ApplicationMeditation *app,
                                                     std::list<Option> &options)
{
    options.emplace_back(std::make_unique<gui::option::OptionSettings>(
        toString(duration),
        [=](const gui::Item &item) {
            if (!app->state->setPreparationTime(duration)) {
                LOG_ERROR("Incorrect preparation time value! The default value is set.");
            }
            refreshOptions(buildOptionsList());
            return true;
        },
        nullptr,
        this,
        app->state->getPreparationTime() == duration ? gui::option::SettingRightItem::Checked
                                                     : gui::option::SettingRightItem::Disabled));
}

void PreparationTimeWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
{
    selectedItemIndex = getSelectedItemIndex();
    refreshOptions(buildOptionsList(), selectedItemIndex);
}

std::size_t PreparationTimeWindow::getSelectedItemIndex() const
{
    auto app                = static_cast<app::ApplicationMeditation *>(application);
    const auto selectedTime = app->state->getPreparationTime();
    const auto it           = std::find(
        std::begin(Constants::Params::preparationTimes), std::end(Constants::Params::preparationTimes), selectedTime);
    return it == std::end(Constants::Params::preparationTimes)
               ? 0
               : std::distance(std::begin(Constants::Params::preparationTimes), it);
}
