// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatWindowPresenter.hpp"

namespace app::alarmClock
{

    CustomRepeatWindowPresenter::CustomRepeatWindowPresenter(std::shared_ptr<CustomRepeatListItemProvider> itemProvider)
        : customRepeatProvider{std::move(itemProvider)}
    {}

    std::shared_ptr<gui::ListItemProvider> CustomRepeatWindowPresenter::getItemProvider()
    {
        return customRepeatProvider;
    }

    void CustomRepeatWindowPresenter::loadData(const WeekDaysRepeatData &data)
    {
        customRepeatProvider->loadData(data);
    }

    WeekDaysRepeatData CustomRepeatWindowPresenter::getWeekDaysRepeatData()
    {
        auto weekDaysOptData = WeekDaysRepeatData();
        auto isCheckedData   = customRepeatProvider->getIsCheckedData();
        uint32_t i           = 0;
        for (const auto &checked : isCheckedData) {
            weekDaysOptData.setData(i, checked);
            ++i;
        }
        return weekDaysOptData;
    }
} // namespace app::alarmClock
