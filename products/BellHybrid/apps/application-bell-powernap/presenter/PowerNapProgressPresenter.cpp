// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressPresenter.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapCommon.hpp"
#include "widgets/PowerNapAlarm.hpp"

#include <apps-common/widgets/ProgressTimer.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace
{
    inline constexpr auto powernapAlarmTimerName = "PowerNapAlarmTimer";
    inline constexpr std::chrono::minutes powernapAlarmTimeout{3};
} // namespace
namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(app::ApplicationCommon *app,
                                                         settings::Settings *settings,
                                                         PowerNapAlarm &alarm)
        : app{app}, settings{settings}, alarm{alarm},
          napAlarmTimer{sys::TimerFactory::createSingleShotTimer(
              app, powernapAlarmTimerName, powernapAlarmTimeout, [this](sys::Timer &) { onNapAlarmFinished(); })}

    {}

    void PowerNapProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onNapFinished(); });
    }

    void PowerNapProgressPresenter::activate()
    {
        Expects(timer != nullptr);
        const auto value = settings->getValue(powernapDBRecordName, settings::SettingsScope::AppLocal);
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
    }
    void PowerNapProgressPresenter::endNap()
    {
        timer->stop();
        napAlarmTimer.stop();
        onNapAlarmFinished();
    }
    void PowerNapProgressPresenter::onNapFinished()
    {
        alarm.start();
        napAlarmTimer.start();
    }
    void PowerNapProgressPresenter::onNapAlarmFinished()
    {
        alarm.stop();
        getView()->napEnded();
    }

} // namespace app::powernap
