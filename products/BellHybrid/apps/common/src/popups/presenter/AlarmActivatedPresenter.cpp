// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/popups/presenter/AlarmActivatedPresenter.hpp>

#include <Timers/TimerFactory.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::popup
{
    AlarmActivatedPresenter::AlarmActivatedPresenter(AbstractAlarmModel &alarmModel, AbstractBatteryModel &batteryModel)
        : alarmModel{alarmModel}, batteryModel{batteryModel}
    {}

    bool AlarmActivatedPresenter::isAlarmActive() const noexcept
    {
        return alarmModel.isActive();
    }

    time_t AlarmActivatedPresenter::getAlarmTime() const noexcept
    {
        return alarmModel.getAlarmTime();
    }

    Layout AlarmActivatedPresenter::getLayout()
    {
        if (layout == Layout::Undefined) {
            const auto batteryStatus = batteryModel.getLevelState();
            const units::SOC soc     = batteryStatus.level;
            const auto state         = batteryStatus.state;
            if (not batteryModel.isBatteryCharging(state) && soc < constants::lowBatteryInfoThreshold) {
                layout = Layout::LowBatteryInfo;
            }
            else {
                layout = Layout::AlarmInfo;
            }
        }
        return layout;
    }

    void AlarmActivatedPresenter::lowBatteryInfoHandled()
    {
        layout = Layout::AlarmInfo;
    }

} // namespace app::popup
