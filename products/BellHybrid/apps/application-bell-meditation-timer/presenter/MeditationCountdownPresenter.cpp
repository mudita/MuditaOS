// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationCountdownPresenter.hpp"

#include <common/LanguageUtils.hpp>
#include <service-db/Settings.hpp>

namespace
{
    constexpr inline auto defaultValue = 30U;
}

namespace app::meditation
{
    MeditationCountdownPresenter::MeditationCountdownPresenter(app::ApplicationCommon *app,
                                                               settings::Settings *settings)
        : app{app}, settings{settings}
    {
        auto countdownDurationStr =
            settings->getValue(meditationCountdownDBRecordName, settings::SettingsScope::AppLocal);

        if (!countdownDurationStr.empty()) {
            duration = std::chrono::seconds{utils::getNumericValue<int>(countdownDurationStr)};
        }
        else {

            duration = std::chrono::seconds{defaultValue};
        }
    }

    void MeditationCountdownPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onCountdownFinished(); });
    }

    void MeditationCountdownPresenter::start()
    {
        if (duration == std::chrono::seconds{0}) {
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

    void MeditationCountdownPresenter::onCountdownFinished()
    {
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;

        app->switchWindow(meditation::windows::meditationProgress, std::move(data));
    }
} // namespace app::meditation
