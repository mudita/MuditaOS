// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationProgressPresenter.hpp"
#include "MeditationCommon.hpp"

#include <apps-common/widgets/ProgressTimerImpl.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

#include <log/log.hpp>
#include <gsl/assert>

namespace
{
    inline constexpr auto meditationProgressTimerName = "MeditationProgressTimer";
    inline constexpr std::chrono::seconds baseTick{1};
} // namespace

namespace app::meditation
{
    MeditationProgressPresenter ::MeditationProgressPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}, model{std::make_shared<MeditationProgressModel>()}
    {
        model->createData();
    }

    void MeditationProgressPresenter::activate(MeditationItem &item)
    {
        model->setData(item);

        settings->setValue(
            meditationDBRecordName, utils::to_string(item.getTimer().count()), settings::SettingsScope::AppLocal);
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
        if (model->getInterval().count() > 0) {
            getView()->intervalReached();
        }
    }

    void MeditationProgressPresenter::onBaseTickReached()
    {
        model->onElapsed();
        getView()->baseTickReached();
    }
} // namespace app::meditation
