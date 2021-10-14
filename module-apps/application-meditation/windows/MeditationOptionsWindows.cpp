// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"
#include "MeditationOptionsWindows.hpp"
#include "Names.hpp"

#include <OptionSetting.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

namespace
{
    using namespace std::chrono_literals;
    constexpr std::array<std::chrono::seconds, 7> preparationTimes{5s, 10s, 30s, 1min, 2min, 5min, 10min};

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
            app->state->showCounter = !app->state->showCounter;
            refreshOptions(buildOptionsList());
            return true;
        },
        [=](gui::Item &item) {
            if (item.focus) {
                setBottomBarText(utils::translate(style::strings::common::Switch), BottomBar::Side::CENTER);
            }
            return true;
        },
        nullptr,
        app->state->showCounter ? option::SettingRightItem::On : option::SettingRightItem::Off));
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
                setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
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
    bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
    addOptions(buildOptionsList());
}

std::list<Option> PreparationTimeWindow::buildOptionsList()
{
    auto app = static_cast<app::ApplicationMeditation *>(application);
    std::list<Option> options;
    for (auto &&duration : preparationTimes) {
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
            app->state->preparationTime = duration;
            refreshOptions(buildOptionsList());
            return true;
        },
        nullptr,
        this,
        app->state->preparationTime == duration ? gui::option::SettingRightItem::Checked
                                                : gui::option::SettingRightItem::Disabled));
}

void PreparationTimeWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
{
    selectedItemIndex = getSelectedItemIndex();
    refreshOptions(buildOptionsList(), selectedItemIndex);
}

unsigned int PreparationTimeWindow::getSelectedItemIndex() const
{
    auto app                = static_cast<app::ApplicationMeditation *>(application);
    const auto selectedTime = app->state->preparationTime;
    for (unsigned int i = 0; i < preparationTimes.size(); ++i) {
        if (selectedTime == preparationTimes[i]) {
            return i;
        }
    }
    return 0;
}
