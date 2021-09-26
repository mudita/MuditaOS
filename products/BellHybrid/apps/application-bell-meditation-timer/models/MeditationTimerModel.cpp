// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerModel.hpp"

#include <i18n/i18n.hpp>

namespace app::meditation
{
    auto MeditationTimerModel::setOnTimerChanged(std::function<void()> cb) -> void
    {
        onTimerChangedCallback = std::move(cb);
    }

    auto MeditationTimerModel::getTimer() -> std::chrono::seconds
    {
        if (meditationItem == nullptr) {
            return std::chrono::seconds{0};
        }
        return meditationItem->getTimer();
    }

    void MeditationTimerModel::increaseTimer()
    {
        if (meditationItem == nullptr) {
            return;
        }
        if (meditationItem->getTimer() >= app::meditation::value::maxTimerValue) {
            return;
        }
        meditationItem->setTimer(meditationItem->getTimer() + std::chrono::minutes{1});
        if (onTimerChangedCallback != nullptr) {
            onTimerChangedCallback();
        }
    }

    void MeditationTimerModel::decreaseTimer()
    {
        if (meditationItem == nullptr) {
            return;
        }
        if (meditationItem->getTimer() <= app::meditation::value::minTimerValue) {
            return;
        }
        meditationItem->setTimer(meditationItem->getTimer() - std::chrono::minutes{1});
        if (onTimerChangedCallback != nullptr) {
            onTimerChangedCallback();
        }
    }

    auto MeditationTimerModel::getTimerString() -> std::string
    {
        if (meditationItem == nullptr) {
            return "";
        }
        return std::to_string(meditationItem->getTimer().count() / 60);
    }
} // namespace app::meditation
