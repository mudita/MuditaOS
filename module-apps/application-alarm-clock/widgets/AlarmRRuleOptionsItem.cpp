// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmRRuleOptionsItem.hpp"

#include <application-alarm-clock/widgets/AlarmClockStyle.hpp>

namespace gui
{
    AlarmRRuleOptionsItem::AlarmRRuleOptionsItem(app::ApplicationCommon *app,
                                                 const std::string &description,
                                                 std::shared_ptr<app::alarmClock::AlarmRRulePresenter> presenter,
                                                 std::function<void(const UTF8 &text)> navBarTemporaryMode,
                                                 std::function<void()> navBarRestoreFromTemporaryMode)
        : AlarmOptionsItem(description), AlarmRRuleItem(std::move(presenter)), app(app),
          navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {
        printOptions();

        inputCallback = [&]([[maybe_unused]] gui::Item &item, const gui::InputEvent &event) {
            const auto ret = optionSpinner->onInput(event);

            if (isCurrentSpinnerOptionCustom()) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::edit));
                if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                    this->app->switchWindow(style::alarmClock::window::name::customRepeat);
                }
            }
            else {
                this->navBarRestoreFromTemporaryMode();
            }

            return ret;
        };

        focusChangedCallback = [&]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);

            if (focus && isCurrentSpinnerOptionCustom()) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::edit));
            }
            else {
                this->navBarRestoreFromTemporaryMode();
            }

            return true;
        };

        onSaveCallback = [&]([[maybe_unused]] std::shared_ptr<AlarmEventRecord> alarm) {
            if (!isCurrentSpinnerOptionCustom()) {
                getPresenter()->setOption(getRRuleOption(optionSpinner->getCurrentValue()));
            }
        };

        onLoadCallback = [&]([[maybe_unused]] std::shared_ptr<AlarmEventRecord> alarm) {
            checkCustomOption(getPresenter()->getDescription());
            optionSpinner->setCurrentValue(getPresenter()->getDescription());

            if (optionSpinner->focus) {
                if (isCurrentSpinnerOptionCustom()) {
                    this->navBarTemporaryMode(utils::translate(style::strings::common::edit));
                }
                else {
                    this->navBarRestoreFromTemporaryMode();
                }
            }
        };
    }

    AlarmRRuleOptionsItem::RRule AlarmRRuleOptionsItem::getRRuleOption(const std::string &selectedOption)
    {
        for (auto const &option : rRuleOptions) {
            if (option.second == selectedOption) {
                return option.first;
            }
        }

        return AlarmRRuleOptionsItem::RRule::Never;
    }

    void AlarmRRuleOptionsItem::checkCustomOption(const std::string &selectedOption)
    {
        if (rRuleOptions.back().second != utils::translate("app_alarm_clock_repeat_custom")) {
            if (const auto days = getPresenter()->getCustomDays();
                std::none_of(days.cbegin(), days.cend(), [](const auto &day) { return day.second; })) {
                rRuleOptions.back().second = utils::translate("app_alarm_clock_repeat_custom");
                printOptions();
            }
        }

        for (auto const &option : rRuleOptions) {
            if (selectedOption.empty() || option.second == selectedOption) {
                return;
            }
        }

        // replace Custom with days options
        rRuleOptions.back().second = selectedOption;
        printOptions();
    }

    void AlarmRRuleOptionsItem::printOptions()
    {
        std::vector<UTF8> printOptions;
        printOptions.reserve(rRuleOptions.size());
        for (auto const &option : rRuleOptions) {
            printOptions.push_back(option.second);
        }
        optionSpinner->setData(printOptions);
    }

    bool AlarmRRuleOptionsItem::isCurrentSpinnerOptionCustom()
    {
        return (getRRuleOption(optionSpinner->getCurrentValue()) == RRule::Custom);
    }
} /* namespace gui */
