// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalBox.hpp"
#include "Style.hpp"
#include "TimerProperty.hpp"

#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <Constants.hpp>
#include <cassert>

using namespace gui;

IntervalBox::IntervalBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TimerProperty *timerSetter)
    : HBox(parent, x, y, w, h), timerSetter(timerSetter)
{
    assert(timerSetter);
    build();
}

void IntervalBox::build()
{
    chimeSpinner = new gui::TextSpinnerBoxWithLabel(
        this, utils::translate("app_meditation_interval_chime"), {}, Boundaries::Continuous);
    chimeSpinner->setMinimumSize(widgetArea.w, widgetArea.h);
    updateIntervals();

    resizeItems();

    focusChangedCallback = [&](gui::Item &item) {
        if (focus) {
            updateIntervals();
        }
        setFocusItem(focus ? chimeSpinner : nullptr);
        return true;
    };
}

void IntervalBox::updateIntervals()
{
    auto setTime = timerSetter->getTime();

    std::vector<UTF8> chimeOptions;
    for (auto chime : Constants::Params::chimeIntervals) {
        if (chime < setTime) {
            chimeOptions.push_back(intervalToString(chime));
        }
    }

    auto currentInterval = chimeSpinner->getCurrentValue();
    chimeSpinner->setData(chimeOptions);

    if (stringToInterval(currentInterval) < setTime) {
        chimeSpinner->setCurrentValue(currentInterval);
    }
}

std::chrono::seconds IntervalBox::getIntervalValue() noexcept
{
    if (stringToInterval(chimeSpinner->getCurrentValue()) < timerSetter->getTime()) {
        return std::chrono::seconds{stringToInterval(chimeSpinner->getCurrentValue())};
    }
    else {
        return std::chrono::seconds{0};
    }
}

bool IntervalBox::setIntervalValue(std::chrono::minutes newValue)
{
    bool result = true;
    const auto it =
        std::find(std::begin(Constants::Params::chimeIntervals), std::end(Constants::Params::chimeIntervals), newValue);
    if (it == std::end(Constants::Params::chimeIntervals)) {
        newValue = Constants::Params::defaultChimeInterval;
        result   = false;
    }

    if (const auto setTime = timerSetter->getTime(); newValue < setTime) {
        chimeSpinner->setCurrentValue(intervalToString(newValue));
    }
    return result;
}

void IntervalBox::setOnChangeCallback(OnChangeCallback &callback)
{
    if (chimeSpinner != nullptr) {
        chimeSpinner->setOnValueChangeCallback([&](const UTF8 &newValue) {
            if (callback != nullptr) {
                callback(stringToInterval(newValue));
            }
        });
    }
}

std::string IntervalBox::intervalToString(std::chrono::minutes time)
{
    if (time.count() == 0) {
        return utils::translate("app_meditation_interval_none");
    }
    const std::string toReplace = "%0";
    std::string description     = utils::translate("app_meditation_interval_every_x_minutes");
    description.replace(description.find(toReplace), toReplace.size(), std::to_string(static_cast<int>(time.count())));
    return description;
}

std::chrono::minutes IntervalBox::stringToInterval(const std::string &description)
{
    if (description == utils::translate("app_meditation_interval_none")) {
        return std::chrono::minutes(0);
    }

    auto number = description;
    number.erase(std::remove_if(number.begin(), number.end(), [](char c) { return !isdigit(c, std::locale()); }),
                 number.end());

    return std::chrono::minutes(std::atoi(number.c_str()));
}
