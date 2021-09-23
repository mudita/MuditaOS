// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeModel.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace app::meditation
{
    auto IntervalChimeModel::setOnIntervalChanged(std::function<void()> cb) -> void
    {
        onIntervalChangedCallback = std::move(cb);
    }

    auto IntervalChimeModel::getInterval() -> std::chrono::seconds
    {
        if (meditationItem == nullptr) {
            return std::chrono::seconds{0};
        }
        return meditationItem->getInterval();
    }

    void IntervalChimeModel::nextInterval()
    {
        if (meditationItem == nullptr) {
            return;
        }
        if (interval == IntervalType::Interval_15) {
            interval = IntervalType::IntervalNone;
        }
        else {
            interval = IntervalType(static_cast<uint32_t>(interval) + 1);
        }
        meditationItem->setInterval(intervalToSecs(interval));
        if (onIntervalChangedCallback != nullptr) {
            onIntervalChangedCallback();
        }
    }

    void IntervalChimeModel::previousInterval()
    {
        if (meditationItem == nullptr) {
            return;
        }
        if (interval == IntervalType::IntervalNone) {
            interval = IntervalType::Interval_15;
        }
        else {
            interval = IntervalType(static_cast<uint32_t>(interval) - 1);
        }
        meditationItem->setInterval(intervalToSecs(interval));
        if (onIntervalChangedCallback != nullptr) {
            onIntervalChangedCallback();
        }
    }

    auto IntervalChimeModel::getIntervalString() -> std::string
    {
        if (meditationItem == nullptr) {
            return "";
        }
        if (interval == IntervalType::IntervalNone) {
            return utils::translate("app_meditation_bell_interval_none");
        }

        const std::string toReplace = "%0";
        std::string temp            = utils::translate("app_meditation_bell_interval_every_x_minutes");

        switch (interval) {
        case IntervalType::IntervalNone:
            break;
        case IntervalType::Interval_1:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(1));
            break;
        case IntervalType::Interval_2:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(2));
            break;
        case IntervalType::Interval_5:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(5));
            break;
        case IntervalType::Interval_10:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(10));
            break;
        case IntervalType::Interval_15:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(15));
            break;
        }
        return temp;
    }

    IntervalChimeModel::IntervalType IntervalChimeModel::secsToInterval(std::chrono::seconds secs)
    {
        int s = secs.count();
        if (s >= intervalToSecs(IntervalType::Interval_15).count()) {
            return IntervalType::Interval_15;
        }
        if (s >= intervalToSecs(IntervalType::Interval_10).count()) {
            return IntervalType::Interval_10;
        }
        if (s >= intervalToSecs(IntervalType::Interval_5).count()) {
            return IntervalType::Interval_5;
        }
        if (s >= intervalToSecs(IntervalType::Interval_2).count()) {
            return IntervalType::Interval_2;
        }
        if (s >= intervalToSecs(IntervalType::Interval_1).count()) {
            return IntervalType::Interval_1;
        }
        return IntervalType::IntervalNone;
    }

    std::chrono::seconds IntervalChimeModel::intervalToSecs(IntervalType interval)
    {
        switch (interval) {
        case IntervalType::IntervalNone:
            return std::chrono::seconds::zero();
        case IntervalType::Interval_1:
            return std::chrono::seconds(std::chrono::minutes{1});
        case IntervalType::Interval_2:
            return std::chrono::seconds(std::chrono::minutes{2});
        case IntervalType::Interval_5:
            return std::chrono::seconds(std::chrono::minutes{5});
        case IntervalType::Interval_10:
            return std::chrono::seconds(std::chrono::minutes{10});
        case IntervalType::Interval_15:
            return std::chrono::seconds(std::chrono::minutes{15});
        }
        return std::chrono::seconds::zero();
    }
} // namespace app::meditation
