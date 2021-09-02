// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockEditWindowPresenter.hpp"

namespace app::alarmClock
{
    AlarmClockEditWindowPresenter::AlarmClockEditWindowPresenter(
        std::shared_ptr<AlarmsInternalListItemProvider> itemProvider)
        : alarmFieldsProvider{std::move(itemProvider)}
    {}

    std::shared_ptr<gui::ListItemProvider> AlarmClockEditWindowPresenter::getAlarmsItemProvider() const
    {
        return alarmFieldsProvider;
    }

    void AlarmClockEditWindowPresenter::loadData(std::shared_ptr<AlarmEventRecord> record)
    {
        alarmFieldsProvider->loadData(std::move(record));
    }

    void AlarmClockEditWindowPresenter::saveData(std::shared_ptr<AlarmEventRecord> record, AlarmAction action)
    {
        alarmFieldsProvider->saveData(std::move(record), action);
    }

    void AlarmClockEditWindowPresenter::loadRepeat(std::shared_ptr<AlarmEventRecord> record)
    {
        alarmFieldsProvider->loadRepeat(std::move(record));
    }

    void AlarmClockEditWindowPresenter::updateRepeat(std::shared_ptr<AlarmEventRecord> record, WeekDaysRepeatData data)
    {
    }
} // namespace app::alarmClock
