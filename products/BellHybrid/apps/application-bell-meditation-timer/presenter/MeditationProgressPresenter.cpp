// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationProgressPresenter.hpp"

#include <apps-common/widgets/ProgressTimerImpl.hpp>

#include "log.hpp"
#include <gsl/assert>

namespace
{
    inline constexpr auto meditationProgressTimerName = "MeditationProgressTimer";
    inline constexpr std::chrono::seconds baseTick{1};
} // namespace

namespace app::meditation
{
    MeditationProgressPresenter ::MeditationProgressPresenter(app::ApplicationCommon *app)
        : app{app}, model{std::make_shared<MeditationProgressModel>()}
    {
        model->createData();
    }

    void MeditationProgressPresenter::activate(MeditationItem &item)
    {
        model->setData(item);
    }

    void MeditationProgressPresenter::request(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }

    void MeditationProgressPresenter::initTimer(gui::Item *parent)
    {
        timer = std::make_unique<app::ProgressTimerImpl>(app, parent, meditationProgressTimerName, baseTick);
        // timer = new app::ProgressTimerImpl(app, parent, meditationProgressTimerName, baseTick);
        timer->registerOnFinishedCallback([this]() { onProgressFinished(); });
        timer->registerOnIntervalCallback([this]() { onIntervalReached(); });
        timer->registerOnBaseTickCallback([this]() { onBaseTickReached(); });
    }

    app::ProgressTimerUIConfigurator &MeditationProgressPresenter::getUIConfigurator() noexcept
    {
        Expects(timer != nullptr);
        return *timer;
    }

    void MeditationProgressPresenter::start()
    {
        timer->reset(model->getDuration(), model->getInterval());
        timer->start();
    }

    void MeditationProgressPresenter::stop()
    {
        timer->stop();
    }

    void MeditationProgressPresenter::pause()
    {
        timer->stop();
    }

    void MeditationProgressPresenter::resume()
    {
        timer->reset(model->getDuration(), model->getInterval(), model->getElapsed());
        timer->start();
    }

    void MeditationProgressPresenter::onProgressFinished()
    {
        getView()->pregressFinished();
    }

    void MeditationProgressPresenter::onIntervalReached()
    {
        getView()->intervalReached();
    }

    void MeditationProgressPresenter::onBaseTickReached()
    {
        model->onElapsed();
        getView()->baseTickReached();
    }
} // namespace app::meditation
