// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/popups/presenter/AlarmActivatedPresenter.hpp>

#include <Timers/TimerFactory.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::popup
{
    AlarmActivatedPresenter ::AlarmActivatedPresenter(std::shared_ptr<AbstractAlarmModel> alarmModel)
        : alarmModel{std::move(alarmModel)}
    {}

    bool AlarmActivatedPresenter::isAlarmActive() const noexcept
    {
        return alarmModel->isActive();
    }

    time_t AlarmActivatedPresenter::getAlarmTime() const noexcept
    {
        return alarmModel->getAlarmTime();
    }

    void AlarmActivatedPresenter::activate()
    {
        return alarmModel->activate(true);
    }

    void AlarmActivatedPresenter::deactivate()
    {
        return alarmModel->activate(false);
    }

    void AlarmActivatedPresenter::updateAlarmModel(AlarmModelReadyHandler callback)
    {
        alarmModel->update(callback);
    }
} // namespace app::popup
