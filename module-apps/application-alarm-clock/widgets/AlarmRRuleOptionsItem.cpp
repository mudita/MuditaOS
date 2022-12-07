// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            auto ret = optionSpinner->onInput(event);

            if (getRRuleOption(optionSpinner->getCurrentValue()) == RRule::Custom) {
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

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);

            if (getRRuleOption(optionSpinner->getCurrentValue()) == RRule::Custom) {
                this->navBarTemporaryMode(utils::translate(style::strings::common::edit));
            }
            else {
                this->navBarRestoreFromTemporaryMode();
            }

            return true;
        };

        onSaveCallback = [&]([[maybe_unused]] std::shared_ptr<AlarmEventRecord> alarm) {
            if (getRRuleOption(optionSpinner->getCurrentValue()) != RRule::Custom) {
                getPresenter()->setOption(getRRuleOption(optionSpinner->getCurrentValue()));
            }
        };

        onLoadCallback = [&]([[maybe_unused]] std::shared_ptr<AlarmEventRecord> alarm) {
            checkCustomOption(getPresenter()->getDescription());
            optionSpinner->setCurrentValue(getPresenter()->getDescription());
            this->navBarRestoreFromTemporaryMode();
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
        for (auto const &option : rRuleOptions) {
            printOptions.push_back(option.second);
        }
        optionSpinner->setData({printOptions});
    }
} /* namespace gui */
