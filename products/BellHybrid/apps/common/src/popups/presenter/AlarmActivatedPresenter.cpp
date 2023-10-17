// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/popups/presenter/AlarmActivatedPresenter.hpp>

#include <Timers/TimerFactory.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::popup
{
    AlarmActivatedPresenter::AlarmActivatedPresenter(AbstractAlarmModel &alarmModel) : alarmModel{alarmModel}
    {}

    bool AlarmActivatedPresenter::isAlarmActive() const noexcept
    {
        return alarmModel.isActive();
    }

    time_t AlarmActivatedPresenter::getAlarmTime() const noexcept
    {
        return alarmModel.getAlarmTime();
    }

} // namespace app::popup
