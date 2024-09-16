// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/popups/presenter/AlarmActivatedPresenter.hpp>
#include <Timers/TimerFactory.hpp>

namespace app::popup
{
    AlarmActivatedPresenter::AlarmActivatedPresenter(AbstractAlarmModel &alarmModel) : alarmModel{alarmModel}
    {}

    bool AlarmActivatedPresenter::isAlarmActive() const noexcept
    {
        return alarmModel.isActive();
    }

    std::time_t AlarmActivatedPresenter::getAlarmTime() const noexcept
    {
        return alarmModel.getAlarmTime();
    }
} // namespace app::popup
