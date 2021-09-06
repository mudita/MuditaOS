// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

    void CustomRepeatWindowPresenter::loadData()
    {
        customRepeatProvider->loadData();
    }

    void CustomRepeatWindowPresenter::eraseProviderData()
    {
        customRepeatProvider->eraseData();
    }

    void CustomRepeatWindowPresenter::saveData()
    {
        customRepeatProvider->saveCheckedData();
    }
} // namespace app::alarmClock
