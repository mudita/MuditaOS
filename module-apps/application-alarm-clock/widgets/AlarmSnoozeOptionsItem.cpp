// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSnoozeOptionsItem.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    AlarmSnoozeOptionsItem::AlarmSnoozeOptionsItem(const std::string &description) : AlarmOptionsItem(description)
    {
        std::vector<UTF8> printOptions;

        for (auto const &option : snoozeOptions) {
            printOptions.push_back(option.second);
        }

        optionSpinner->setData({printOptions});

        onSaveCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            for (auto const &option : snoozeOptions) {
                if (option.second == optionSpinner->getCurrentValue().c_str()) {
                    alarm->snoozeDuration = option.first.count();
                }
            }
        };

        onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            auto valueToLoad = alarm->snoozeDuration;
            for (auto const &option : snoozeOptions) {
                if (option.first.count() == valueToLoad) {
                    optionSpinner->setCurrentValue(option.second);
                }
            }
        };
    }
} /* namespace gui */
