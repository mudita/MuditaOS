// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockMainWindowPresenter.hpp"

namespace app::alarmClock
{
    AlarmClockMainWindowPresenter::AlarmClockMainWindowPresenter(std::shared_ptr<AlarmsListItemProvider> alarmsProvider)
        : alarmsProvider{std::move(alarmsProvider)}
    {}

    std::shared_ptr<gui::ListItemProvider> AlarmClockMainWindowPresenter::getAlarmsItemProvider() const
    {
        return alarmsProvider;
    }
} // namespace app::alarmClock
