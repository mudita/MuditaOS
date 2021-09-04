// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressPresenter.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapCommon.hpp"

#include <apps-common/widgets/ProgressTimerImpl.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace
{
    inline constexpr auto powernapTimerName = "PowerNapTimer";
    inline constexpr auto powernapAlarmTimerName = "PowerNapAlarmTimer";
    inline constexpr std::chrono::seconds timerTick{1};
    inline constexpr std::chrono::minutes powernapAlarmTimeout{3};
} // namespace
namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(app::Application *app, settings::Settings *settings)
        : app{app}, settings{settings}, napAlarmTimer{sys::TimerFactory::createSingleShotTimer(
                                            app, powernapAlarmTimerName, powernapAlarmTimeout, [this](sys::Timer &) {
                                                onNapAlarmFinished();
                                            })}
    {}
    void PowerNapProgressPresenter::initTimer(gui::Item *parent)
    {
        timer = std::make_unique<app::ProgressTimerImpl>(app, parent, powernapTimerName, timerTick);
        timer->registerOnFinishedCallback([this]() { onNapFinished(); });
    }
    void PowerNapProgressPresenter::activate()
    {
        Expects(timer != nullptr);
        const auto value = settings->getValue(powernapDBRecordName, settings::SettingsScope::AppLocal);
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
    }
    app::ProgressTimerUIConfigurator &PowerNapProgressPresenter::getUIConfigurator() noexcept
    {
        Expects(timer != nullptr);
        return *timer;
    }
    void PowerNapProgressPresenter::onNapFinished()
    {
        napAlarmTimer.start();
    }

    void PowerNapProgressPresenter::onNapAlarmFinished()
    {
        getView()->switchWindow();
    }
} // namespace app::powernap
