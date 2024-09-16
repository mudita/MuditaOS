// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "MeditationCommon.hpp"
#include "MeditationCountdownPresenter.hpp"

#include <service-db/Settings.hpp>
#include <LanguageUtils.hpp>

namespace app::meditation
{
    MeditationCountdownPresenter::MeditationCountdownPresenter(app::ApplicationCommon *app,
                                                               models::StartDelay &startDelay)
        : app{app}, startDelayModel{startDelay}, duration{startDelayModel.getValue()}
    {}

    void MeditationCountdownPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onCountdownFinished(); });
    }

    void MeditationCountdownPresenter::start()
    {
        if (duration == std::chrono::seconds::zero()) {
            onCountdownFinished();
        }
        else {
            timer->reset(std::chrono::seconds(duration));
            timer->start();
        }
    }

    void MeditationCountdownPresenter::stop()
    {
        timer->stop();
    }

    bool MeditationCountdownPresenter::isFinished()
    {
        return finished;
    }

    void MeditationCountdownPresenter::setReady(bool status)
    {
        ready = status;
    }

    bool MeditationCountdownPresenter::isReady()
    {
        return ready;
    }

    void MeditationCountdownPresenter::onCountdownFinished()
    {
        finished = true;
        if (ready) {
            app->switchWindow(meditation::windows::meditationProgress);
        }
    }
} // namespace app::meditation
