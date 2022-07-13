// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProgressTimerWithBarGraphAndCounter.hpp"
#include <Text.hpp>
#include <ProgressBar.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/GuiTimer.hpp>
#include <gsl/assert>

namespace
{
    inline constexpr auto increasingModePrefix = "-";
}
namespace app
{
    void ProgressTimerWithBarGraphAndCounter::update()
    {
        updateText();
        updateProgress();
        ProgressTimer::update();
    }

    void ProgressTimerWithBarGraphAndCounter::updateText()
    {
        const auto secondsRemaining = duration - std::chrono::duration_cast<std::chrono::seconds>(elapsed);
        timeWidget->setFirst(secondsRemaining.count() / 60);
        timeWidget->setSecond(secondsRemaining.count() % 60);
    }

    void ProgressTimerWithBarGraphAndCounter::updateProgress()
    {
        if (progress != nullptr) {
            const auto percentage =
                static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()) /
                duration.count();
            const auto currentStep = percentage * progress->getMaximum();
            progress->setValue(std::ceil(currentStep));
        }
    }

    void ProgressTimerWithBarGraphAndCounter::attach(gui::Progress *_progress)
    {
        Expects(_progress != nullptr);
        progress = _progress;
    }

    void ProgressTimerWithBarGraphAndCounter::attach(gui::Text *_text)
    {
        Expects(_text != nullptr);
        text = _text;
    }

    void ProgressTimerWithBarGraphAndCounter::attach(gui::TimeFixedWidget *_timeWidget)
    {
        Expects(_timeWidget != nullptr);
        timeWidget = _timeWidget;
    }
} // namespace app
