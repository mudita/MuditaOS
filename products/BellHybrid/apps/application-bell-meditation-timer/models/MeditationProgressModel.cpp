// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationProgressModel.hpp"

namespace app::meditation
{
    void MeditationProgressModel::onElapsed(std::chrono::seconds elapsed)
    {
        if (meditationItem == nullptr) {
            return;
        }
        meditationItem->setPassed(meditationItem->getPassed() + elapsed);
    }

    auto MeditationProgressModel::getDuration() -> std::chrono::seconds
    {
        if (meditationItem == nullptr) {
            return std::chrono::seconds::zero();
            ;
        }
        return meditationItem->getTimer();
    }

    auto MeditationProgressModel::getInterval() -> std::chrono::seconds
    {
        if (meditationItem == nullptr) {
            return std::chrono::seconds::zero();
            ;
        }
        return meditationItem->getInterval();
    }

    auto MeditationProgressModel::getElapsed() -> std::chrono::seconds
    {
        if (meditationItem == nullptr) {
            return std::chrono::seconds::zero();
            ;
        }
        return meditationItem->getPassed();
    }
} // namespace app::meditation
