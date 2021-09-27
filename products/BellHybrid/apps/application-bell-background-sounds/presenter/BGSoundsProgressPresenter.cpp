// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsProgressPresenter.hpp"
#include "data/BGSoundsCommon.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace app::bgSounds
{
    BGSoundsProgressPresenter::BGSoundsProgressPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : settings{settings}
    {}

    void BGSoundsProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onFinished(); });
    }

    void BGSoundsProgressPresenter::activate()
    {
        Expects(timer != nullptr);
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        runTimer         = utils::is_number(value);
        if (runTimer) {
            timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
            timer->start();
        }
    }
    void BGSoundsProgressPresenter::stop()
    {
        timer->stop();
        onFinished();
    }
    void BGSoundsProgressPresenter::onFinished()
    {
        getView()->onFinished();
    }
    void BGSoundsProgressPresenter::pause()
    {
        if (not timer->isStopped()) {
            timer->stop();
            getView()->onPaused();
        }
    }
    void BGSoundsProgressPresenter::resume()
    {
        if (runTimer && timer->isStopped()) {
            timer->start();
        }
    }

} // namespace app::bgSounds
