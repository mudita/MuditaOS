// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/text/Label.hpp>
#include <apps-common/widgets/TextSpinnerBoxWithLabel.hpp>

#include <list>
#include <chrono>

namespace gui
{
    class TimerProperty;

    class IntervalBox : public HBox
    {
        TimerProperty *timerSetter            = nullptr;
        TextSpinnerBoxWithLabel *chimeSpinner = nullptr;

        void build();
        void updateIntervals();
        [[nodiscard]] std::string intervalToString(std::chrono::minutes time);
        [[nodiscard]] std::chrono::minutes stringToInterval(const std::string &description);

        void rescaleIntervals();

      public:
        IntervalBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TimerProperty *timerSetter);

        [[nodiscard]] std::chrono::seconds getIntervalValue() noexcept;
    };
} // namespace gui
