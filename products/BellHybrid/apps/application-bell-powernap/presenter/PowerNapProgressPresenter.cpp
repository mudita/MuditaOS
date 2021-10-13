// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressPresenter.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapCommon.hpp"

#include <apps-common/widgets/ProgressTimer.hpp>
#include <common/models/TimeModel.hpp>
#include <db/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace
{
    inline constexpr auto powernapAlarmTimerName = "PowerNapAlarmTimer";
    inline constexpr std::chrono::minutes powernapAlarmTimeout{5};
} // namespace

namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(app::ApplicationCommon *app,
                                                         settings::Settings *settings,
                                                         std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                                                         std::unique_ptr<AbstractAudioModel> audioModel,
                                                         std::unique_ptr<AbstractTimeModel> timeModel)
        : app{app}, settings{settings}, soundsRepository{std::move(soundsRepository)},
          audioModel{std::move(audioModel)}, timeModel{std::move(timeModel)},
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
        const auto value = settings->getValue(powernapDBRecordName);
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
    }
    void PowerNapProgressPresenter::endNap()
    {
        napFinished = false;
        timer->stop();
        napAlarmTimer.stop();
        onNapAlarmFinished();
    }
    void PowerNapProgressPresenter::onNapFinished()
    {
        const auto filePath = soundsRepository->titleToPath(
            settings->getValue(bell::settings::Alarm::tone, settings::SettingsScope::Global));
        auto playResponseCb = [this](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            this->currentToken = token;
        };
        audioModel->play(filePath.value_or(""), playResponseCb, AbstractAudioModel::PlaybackType::Alarm);
        napAlarmTimer.start();
        napFinished = true;
    }
    void PowerNapProgressPresenter::onNapAlarmFinished()
    {
        audioModel->stop(currentToken, nullptr);
        getView()->napEnded();
    }

    void PowerNapProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }
    bool PowerNapProgressPresenter::isNapFinished()
    {
        return napFinished;
    }

} // namespace app::powernap
