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
        using utils::time::Duration;
        if (text == nullptr) {
            return;
        }
        const auto secondsRemaining = duration - elapsed;
        const Duration remainingDuration{std::time_t{secondsRemaining.count()}};
        UTF8 timerText;
        if (countdownMode == ProgressCountdownMode::Increasing && secondsRemaining != std::chrono::seconds::zero()) {
            timerText += increasingModePrefix;
        }
        timerText += remainingDuration.str(displayFormat);
        text->setText(std::move(timerText));
    }

    void ProgressTimerWithBarGraphAndCounter::updateProgress()
    {
        if (progress != nullptr) {
            const auto percentage  = static_cast<float>(elapsed.count()) / duration.count();
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
} // namespace app
